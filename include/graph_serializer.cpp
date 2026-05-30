#include "graph_serializer.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

using json = nlohmann::json;

// бинарник - самый экономный, но менее удобный
bool BinaryStrategy::save(const FastModificationGraph& graph, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    
    // заголовок
    uint32_t magic = 0x47524648;
    uint32_t version = 2;  // предыдущая версия была без transfer
    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    writeStations(file, graph);
    writeEdges(file, graph);
    
    return true;
}

FastModificationGraph BinaryStrategy::load(const std::string& filename) {
    FastModificationGraph graph;
    std::ifstream file(filename, std::ios::binary);
    if (!file) return graph;
    
    uint32_t magic, version;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    
    if (magic != 0x47524648) return graph;
    
    readStations(file, graph);
    
    if (version == 1) {
        readEdgesV1(file, graph);  // без is_transfer
    } else {
        readEdges(file, graph);     // с is_transfer
    }
    
    return graph;
}

void BinaryStrategy::writeStations(std::ofstream& file, const FastModificationGraph& graph) {
    uint32_t count = graph.StationCount();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    for (const auto& [id, station] : graph.station_by_id) {
        file.write(reinterpret_cast<const char*>(&id), sizeof(id));
        
        uint32_t name_len = static_cast<uint32_t>(station.getName().size());
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(station.getName().c_str(), name_len);
        
        int line_num = station.getLineNo();
        file.write(reinterpret_cast<const char*>(&line_num), sizeof(line_num));
    }
}

void BinaryStrategy::writeEdges(std::ofstream& file, const FastModificationGraph& graph) {
    uint32_t edge_count = 0;
    for (const auto& [_, edges] : graph.adj_list) {
        edge_count += edges.size();
    }
    file.write(reinterpret_cast<const char*>(&edge_count), sizeof(edge_count));
    
    // Сохраняем все рёбра
    for (const auto& [from, edges] : graph.adj_list) {
        for (const Edge& e : edges) {
            file.write(reinterpret_cast<const char*>(&from), sizeof(from));
            file.write(reinterpret_cast<const char*>(&e.to), sizeof(e.to));
            file.write(reinterpret_cast<const char*>(&e.time), sizeof(e.time));
            file.write(reinterpret_cast<const char*>(&e.cost), sizeof(e.cost));
            file.write(reinterpret_cast<const char*>(&e.is_transfer), sizeof(e.is_transfer));
        }
    }
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

// для обратной совместимости (вдруг откажемся от transfer)
void BinaryStrategy::readEdgesV1(std::ifstream& file, FastModificationGraph& graph) {
    uint32_t edge_count;
    file.read(reinterpret_cast<char*>(&edge_count), sizeof(edge_count));
    
    for (uint32_t i = 0; i < edge_count; ++i) {
        size_t from, to;
        int time, cost;
        
        file.read(reinterpret_cast<char*>(&from), sizeof(from));
        file.read(reinterpret_cast<char*>(&to), sizeof(to));
        file.read(reinterpret_cast<char*>(&time), sizeof(time));
        file.read(reinterpret_cast<char*>(&cost), sizeof(cost));
        
        // В старой версии считаем is_transfer = false
        graph.AddEdge(from, Edge(time, cost, to, false));
    }
}

// JSONSt - баланс читаемости, скорости и масштабирования
bool JSONStrategy::save(const FastModificationGraph& graph, const std::string& filename) {
    json j;
    
    j["version"] = 2;  // версия формата - хороший тон
    
    // схоранение станции
    json stations = json::array();
    for (const auto& [id, station] : graph.station_by_id) {
        stations.push_back({
            {"id", id},
            {"name", station.getName()},
            {"line", station.getLineNo()}
        });
    }
    j["stations"] = stations;
    
    // сохранение рербер с is_transfer
    json edges = json::array();
    for (const auto& [from, edge_set] : graph.adj_list) {
        for (const Edge& e : edge_set) {
            edges.push_back({
                {"from", from},
                {"to", e.to},
                {"time", e.time},
                {"cost", e.cost},
                {"is_transfer", e.is_transfer}
            });
        }
    }
    j["edges"] = edges;
    
    std::ofstream file(filename);
    file << j.dump(2);
    return true;
}

FastModificationGraph JSONStrategy::load(const std::string& filename) {
    FastModificationGraph graph;
    std::ifstream file(filename);
    if (!file) return graph;
    
    json j;
    file >> j;
    
    // загружаем станции
    for (const auto& s : j["stations"]) {
        size_t id = s["id"];
        std::string name = s["name"];
        int line = s.value("line", 0);
        graph.AddStation(Station(name, line, id));
    }
    
    // загружаем рёбра
    for (const auto& e : j["edges"]) {
        size_t from = e["from"];
        size_t to = e["to"];
        int time = e.value("time", 0);
        int cost = e.value("cost", 1);
        bool is_transfer = e.value("is_transfer", false);  // Обратная совместимость
        
        graph.AddEdge(from, Edge(time, cost, to, is_transfer));
    }
    
    return graph;
}

// самый тупой и простой формат
bool TextStrategy::save(const FastModificationGraph& graph, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) return false;
    
    // "# STATION id name line\n";
    // "# EDGE from to time cost is_transfer (0/1)\n\n";
    
    for (const auto& [id, station] : graph.station_by_id) {
        file << "STATION " << id << " " << station.getName() << " " << station.getLineNo() << "\n";
    }
    
    file << "\n";
    
    for (const auto& [from, edges] : graph.adj_list) {
        for (const Edge& e : edges) {
            file << "EDGE " << from << " " << e.to << " " 
                 << e.time << " " << e.cost << " "
                 << (e.is_transfer ? 1 : 0) << "\n";
        }
    }
    
    return true;
}

FastModificationGraph TextStrategy::load(const std::string& filename) {
    FastModificationGraph graph;
    std::ifstream file(filename);
    if (!file) return graph;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
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
            
            // попытка прочитать is_transfer (для обратной совместимости)
            if (iss >> is_transfer) {
                // норм
            } else {
                is_transfer = 0;  // х#### заменяем на 0
            }
            
            graph.AddEdge(from, Edge(time, cost, to, is_transfer != 0));
        }
    }
    
    return graph;
}

// сериализатор для разделения ответственности
GraphSerializer::GraphSerializer() {
    // по умолчанию пусть будет бинарник для скорости
    strategy = std::make_unique<BinaryStrategy>();
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