#include "graph_serializer.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

using json = nlohmann::json;

// бинарник - самый экономный, но менее удобный
bool BinaryStrategy::save(const FastModificationGraph& graph, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }
    uint32_t magic = 0x47524648;
    uint32_t version = 2;
    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    writeStations(file, graph);
    writeEdges(file, graph);
    
    return true;
}

FastModificationGraph BinaryStrategy::load(const std::string& filename) {
    FastModificationGraph graph;
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw FileNotFoundException(filename);
    }
    uint32_t magic, version;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    
    if (magic != 0x47524648) {
        throw InvalidFormatException(filename);
    }
    if (version != 2) {
        throw UnsupportedVersionException(version, 2);
    }
    readStations(file, graph);
    readEdges(file, graph);
    
    return graph;
}

void BinaryStrategy::writeStations(std::ofstream& file, const FastModificationGraph& graph) {
    uint32_t count = graph.getStationCount();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));    
    graph.forEachVertex([&](const Station& station) {
        size_t id = station.getId();
        file.write(reinterpret_cast<const char*>(&id), sizeof(id));
        
        uint32_t name_len = static_cast<uint32_t>(station.getName().size());
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(station.getName().c_str(), name_len);
        
        int line_num = station.getLineNo();
        file.write(reinterpret_cast<const char*>(&line_num), sizeof(line_num));
        
        return true;
    });
}

void BinaryStrategy::writeEdges(std::ofstream& file, const FastModificationGraph& graph) {
    uint32_t edge_count = 0;
    graph.forEachVertex([&](const Station& station) {
        size_t from = station.getId();
        graph.forEachEdge(from, [&](const Edge& e) {
            edge_count++;
            return true;
        });
        return true;
    });
    file.write(reinterpret_cast<const char*>(&edge_count), sizeof(edge_count));    
    graph.forEachVertex([&](const Station& station) {
        size_t from = station.getId();
        graph.forEachEdge(from, [&](const Edge& e) {
            file.write(reinterpret_cast<const char*>(&from), sizeof(from));
            file.write(reinterpret_cast<const char*>(&e.to), sizeof(e.to));
            file.write(reinterpret_cast<const char*>(&e.time), sizeof(e.time));
            file.write(reinterpret_cast<const char*>(&e.cost), sizeof(e.cost));
            file.write(reinterpret_cast<const char*>(&e.is_transfer), sizeof(e.is_transfer));
            return true;
        });
        return true;
    });
}

void BinaryStrategy::readStations(std::ifstream& file, FastModificationGraph& graph) {
    uint32_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (uint32_t i = 0; i < count; ++i) {
        size_t id;
        file.read(reinterpret_cast<char*>(&id), sizeof(id));
        
        uint32_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        std::string name(name_len, '\0');
        file.read(&name[0], name_len);
        
        int line_num;
        file.read(reinterpret_cast<char*>(&line_num), sizeof(line_num));
        
        graph.AddStation(Station(name, line_num, id));
    }
}

void BinaryStrategy::readEdges(std::ifstream& file, FastModificationGraph& graph) {
    uint32_t edge_count;
    file.read(reinterpret_cast<char*>(&edge_count), sizeof(edge_count));
    
    for (uint32_t i = 0; i < edge_count; ++i) {
        size_t from, to;
        int time, cost;
        bool is_transfer;
        
        file.read(reinterpret_cast<char*>(&from), sizeof(from));
        file.read(reinterpret_cast<char*>(&to), sizeof(to));
        file.read(reinterpret_cast<char*>(&time), sizeof(time));
        file.read(reinterpret_cast<char*>(&cost), sizeof(cost));
        file.read(reinterpret_cast<char*>(&is_transfer), sizeof(is_transfer));
        
        graph.AddEdge(from, Edge(time, cost, to, is_transfer));
    }
}

// JSON - для баланса читаемости, скорости и масштабирования
bool JSONStrategy::save(const FastModificationGraph& graph, const std::string& filename) {
    json j;
    j["version"] = 2;
    
    json stations = json::array();
    graph.forEachVertex([&](const Station& station) {
        stations.push_back({
            {"id", station.getId()},
            {"name", station.getName()},
            {"line", station.getLineNo()}
        });
        return true;
    });
    j["stations"] = stations;    
    json edges = json::array();
    graph.forEachVertex([&](const Station& station) {
        size_t from = station.getId();
        graph.forEachEdge(from, [&](const Edge& e) {
            edges.push_back({
                {"from", from},
                {"to", e.to},
                {"time", e.time},
                {"cost", e.cost},
                {"is_transfer", e.is_transfer}
            });
            return true;
        });
        return true;
    });
    j["edges"] = edges;
    
    std::ofstream file(filename);
    file << j.dump(2);
    return true;
}

FastModificationGraph JSONStrategy::load(const std::string& filename) {
    FastModificationGraph graph;
    std::ifstream file(filename);
    if (!file) {
        throw FileNotFoundException(filename);
    }
    json j;
    file >> j;
    
    for (const auto& s : j["stations"]) {
        size_t id = s["id"];
        std::string name = s["name"];
        int line = s.value("line", 0);
        graph.AddStation(Station(name, line, id));
    }
    
    for (const auto& e : j["edges"]) {
        size_t from = e["from"];
        size_t to = e["to"];
        int time = e.value("time", 0);
        int cost = e.value("cost", 1);
        bool is_transfer = e.value("is_transfer", false);
        
        graph.AddEdge(from, Edge(time, cost, to, is_transfer));
    }
    
    return graph;
}

// самый простой текстовый формат
bool TextStrategy::save(const FastModificationGraph& graph, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw FileNotFoundException(filename);
    }
    graph.forEachVertex([&](const Station& station) {
        file << "STATION " << station.getId() << " " 
             << station.getName() << " " << station.getLineNo() << "\n";
        return true;
    });
    
    file << "\n";
    
    graph.forEachVertex([&](const Station& station) {
        size_t from = station.getId();
        graph.forEachEdge(from, [&](const Edge& e) {
            file << "EDGE " << from << " " << e.to << " " 
                 << e.time << " " << e.cost << " "
                 << (e.is_transfer ? 1 : 0) << "\n";
            return true;
        });
        return true;
    });
    
    return true;
}

FastModificationGraph TextStrategy::load(const std::string& filename) {
    FastModificationGraph graph;
    std::ifstream file(filename);
    if (!file) {
        throw FileNotFoundException(filename);
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        
        if (type == "STATION") {
            size_t id;
            std::string name;
            int line_num;
            iss >> id >> name >> line_num;
            graph.AddStation(Station(name, line_num, id));
        }
        else if (type == "EDGE") {
            size_t from, to;
            int time, cost;
            int is_transfer = 0;
            
            iss >> from >> to >> time >> cost;
            iss >> is_transfer;
            
            graph.AddEdge(from, Edge(time, cost, to, is_transfer != 0));
        }
    }
    
    return graph;
}

// сериализатор для разделения ответственности
GraphSerializer::GraphSerializer() {
    strategy = std::make_unique<JSONStrategy>();
}

void GraphSerializer::setFormat(const std::string& format) {
    auto new_strategy = createStrategy(format);
    if (new_strategy) {
        strategy = std::move(new_strategy);
    }
}

void GraphSerializer::setFormatByExtension(const std::string& filename) {
    size_t dot = filename.find_last_of('.');
    if (dot != std::string::npos) {
        std::string ext = filename.substr(dot + 1);
        
        if (ext == "bin") setFormat("binary");
        else if (ext == "json") setFormat("json");
        else if (ext == "txt") setFormat("text");
    }
}

bool GraphSerializer::save(const FastModificationGraph& graph, const std::string& filename) {
    return strategy->save(graph, filename);
}

FastModificationGraph GraphSerializer::load(const std::string& filename) {
    return strategy->load(filename);
}

std::vector<std::string> GraphSerializer::getAvailableFormats() const {
    return {"binary", "json", "text"};
}

std::unique_ptr<ISerializationStrategy> GraphSerializer::createStrategy(const std::string& format) {
    if (format == "binary") return std::make_unique<BinaryStrategy>();
    if (format == "json") return std::make_unique<JSONStrategy>();
    if (format == "text") return std::make_unique<TextStrategy>();
    return nullptr;
}