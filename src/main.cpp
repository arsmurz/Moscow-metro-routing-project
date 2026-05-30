#include "graph.h"
#include "graph_serializer.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <chrono>

// Нейротесты для базовых функций

namespace fs = std::filesystem;

// ============= Вспомогательные функции =============
void printSeparator() {
    std::cout << "\n========================================\n";
}

void printStation(const Station& s) {
    std::cout << "  Station{id=" << s.getId() 
              << ", name='" << s.getName() 
              << "', line=" << s.getLineNo() << "}\n";
}

void printEdge(const Edge& e, size_t from) {
    std::cout << "  Edge{from=" << from 
              << ", to=" << e.to 
              << ", time=" << e.time 
              << ", cost=" << e.cost 
              << ", is_transfer=" << (e.is_transfer ? "true" : "false") << "}\n";
}

void printGraph(const FastModificationGraph& graph, const std::string& title) {
    std::cout << "\n" << title << ":\n";
    std::cout << "Stations count: " << graph.StationCount() << "\n";
    
    std::cout << "Stations:\n";
    for (const auto& [id, station] : graph.station_by_id) {
        printStation(station);
    }
    
    std::cout << "Edges:\n";
    for (const auto& [from, edges] : graph.adj_list) {
        for (const Edge& e : edges) {
            printEdge(e, from);
        }
    }
}

bool compareGraphs(const FastModificationGraph& g1, const FastModificationGraph& g2) {
    if (g1.StationCount() != g2.StationCount()) {
        std::cout << "Station count mismatch: " << g1.StationCount() << " vs " << g2.StationCount() << "\n";
        return false;
    }
    
    // Сравниваем станции
    for (const auto& [id, station] : g1.station_by_id) {
        auto it = g2.station_by_id.find(id);
        if (it == g2.station_by_id.end()) {
            // Проверяем по имени
            bool found = false;
            for (const auto& [id2, s2] : g2.station_by_id) {
                if (s2.getName() == station.getName()) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "Station " << station.getName() << " not found in second graph\n";
                return false;
            }
        }
    }
    
    // Сравниваем рёбра
    size_t edge_count1 = 0, edge_count2 = 0;
    for (const auto& [from, edges] : g1.adj_list) {
        edge_count1 += edges.size();
    }
    for (const auto& [from, edges] : g2.adj_list) {
        edge_count2 += edges.size();
    }
    
    if (edge_count1 != edge_count2) {
        std::cout << "Edge count mismatch: " << edge_count1 << " vs " << edge_count2 << "\n";
        return false;
    }
    
    return true;
}

// ============= Создание тестового графа =============
FastModificationGraph createTestGraph() {
    FastModificationGraph graph;
    
    graph.AddStation(Station("Central", 1));
    graph.AddStation(Station("North Station", 1));
    graph.AddStation(Station("South Station", 2));
    graph.AddStation(Station("East Station", 2));
    graph.AddStation(Station("West Station", 3));
    
    size_t central = graph.getId("Central");
    size_t north = graph.getId("North Station");
    size_t south = graph.getId("South Station");
    size_t east = graph.getId("East Station");
    size_t west = graph.getId("West Station");
    
    graph.AddEdge(central, Edge(5, 10, north, false));
    graph.AddEdge(central, Edge(3, 7, south, true));
    graph.AddEdge(central, Edge(4, 8, east, false));
    graph.AddEdge(central, Edge(6, 12, west, true));
    graph.AddEdge(north, Edge(4, 5, south, false));
    graph.AddEdge(south, Edge(5, 6, east, true));
    graph.AddEdge(east, Edge(3, 4, west, false));
    graph.AddEdge(west, Edge(7, 9, north, true));
    
    return graph;
}

// ============= Тесты базовых операций =============
void testAddStations() {
    printSeparator();
    std::cout << "Testing AddStation...\n";
    
    FastModificationGraph graph;
    graph.AddStation(Station("Central", 1));
    graph.AddStation(Station("North Station", 1));
    graph.AddStation(Station("South Station", 2));
    
    assert(graph.StationCount() == 3);
    std::cout << "✓ Station count: " << graph.StationCount() << "\n";
    
    assert(graph.getId("Central") != static_cast<size_t>(-1));
    assert(graph.getId("North Station") != static_cast<size_t>(-1));
    assert(graph.getId("South Station") != static_cast<size_t>(-1));
    std::cout << "✓ Station lookup works\n";
}

void testAddEdges() {
    printSeparator();
    std::cout << "Testing AddEdge...\n";
    
    FastModificationGraph graph;
    graph.AddStation(Station("A", 1));
    graph.AddStation(Station("B", 1));
    
    size_t a = graph.getId("A");
    size_t b = graph.getId("B");
    
    graph.AddEdge(a, Edge(5, 10, b, false));
    
    size_t edge_count = 0;
    graph.forEachEdge(a, [&](const Edge& e) {
        edge_count++;
        return true;
    });
    
    assert(edge_count == 1);
    std::cout << "✓ Edge added successfully\n";
}

// ============= Тесты сериализации =============
void testBinarySerialization() {
    printSeparator();
    std::cout << "Testing Binary Serialization...\n";
    
    auto original = createTestGraph();
    std::string filename = "test_binary.bin";
    
    GraphSerializer serializer;
    serializer.setFormat("binary");
    
    auto start = std::chrono::high_resolution_clock::now();
    bool saved = serializer.save(original, filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto save_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(saved && "Failed to save binary file");
    
    start = std::chrono::high_resolution_clock::now();
    auto loaded = serializer.load(filename);
    end = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(compareGraphs(original, loaded) && "Binary: Graphs don't match");
    
    std::cout << "✓ Binary serialization works\n";
    std::cout << "  Save time: " << save_time.count() << " μs\n";
    std::cout << "  Load time: " << load_time.count() << " μs\n";
    std::cout << "  File size: " << fs::file_size(filename) << " bytes\n";
    
    fs::remove(filename);
}

void testJSONSerialization() {
    printSeparator();
    std::cout << "Testing JSON Serialization...\n";
    
    auto original = createTestGraph();
    std::string filename = "test_json.json";
    
    GraphSerializer serializer;
    serializer.setFormat("json");
    
    auto start = std::chrono::high_resolution_clock::now();
    bool saved = serializer.save(original, filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto save_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(saved && "Failed to save JSON file");
    
    start = std::chrono::high_resolution_clock::now();
    auto loaded = serializer.load(filename);
    end = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(compareGraphs(original, loaded) && "JSON: Graphs don't match");
    
    std::cout << "✓ JSON serialization works\n";
    std::cout << "  Save time: " << save_time.count() << " μs\n";
    std::cout << "  Load time: " << load_time.count() << " μs\n";
    std::cout << "  File size: " << fs::file_size(filename) << " bytes\n";
    
    // Выводим содержимое JSON для демонстрации
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::cout << "  JSON preview: " << content.substr(0, 200) << "...\n";
    
    fs::remove(filename);
}

void testTextSerialization() {
    printSeparator();
    std::cout << "Testing Text Serialization...\n";
    
    auto original = createTestGraph();
    std::string filename = "test_text.txt";
    
    GraphSerializer serializer;
    serializer.setFormat("text");
    
    auto start = std::chrono::high_resolution_clock::now();
    bool saved = serializer.save(original, filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto save_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(saved && "Failed to save text file");
    
    start = std::chrono::high_resolution_clock::now();
    auto loaded = serializer.load(filename);
    end = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(compareGraphs(original, loaded) && "Text: Graphs don't match");
    
    std::cout << "✓ Text serialization works\n";
    std::cout << "  Save time: " << save_time.count() << " μs\n";
    std::cout << "  Load time: " << load_time.count() << " μs\n";
    std::cout << "  File size: " << fs::file_size(filename) << " bytes\n";
    
    fs::remove(filename);
}

void testAutoDetection() {
    printSeparator();
    std::cout << "Testing Auto-detection...\n";
    
    auto original = createTestGraph();
    GraphSerializer serializer;
    
    // Сохраняем в разных форматах
    serializer.setFormat("binary");
    serializer.save(original, "auto_test.bin");
    
    serializer.setFormat("json");
    serializer.save(original, "auto_test.json");
    
    serializer.setFormat("text");
    serializer.save(original, "auto_test.txt");
    
    // Загружаем с автоопределением
    GraphSerializer loader;
    
    loader.setFormatByExtension("auto_test.bin");
    auto from_bin = loader.load("auto_test.bin");
    
    loader.setFormatByExtension("auto_test.json");
    auto from_json = loader.load("auto_test.json");
    
    loader.setFormatByExtension("auto_test.txt");
    auto from_text = loader.load("auto_test.txt");
    
    assert(compareGraphs(original, from_bin));
    assert(compareGraphs(original, from_json));
    assert(compareGraphs(original, from_text));
    
    std::cout << "✓ Auto-detection works for all formats\n";
    
    fs::remove("auto_test.bin");
    fs::remove("auto_test.json");
    fs::remove("auto_test.txt");
}

void testBackwardCompatibility() {
    printSeparator();
    std::cout << "Testing Backward Compatibility...\n";
    
    // Создаём старый граф (версия 1, без is_transfer)
    FastModificationGraph old_graph;
    old_graph.AddStation(Station("Old Central", 1));
    old_graph.AddStation(Station("Old North", 1));
    
    size_t central = old_graph.getId("Old Central");
    size_t north = old_graph.getId("Old North");
    
    old_graph.AddEdge(central, Edge(5, 10, north, false));
    
    // Сохраняем в старом формате через бинарную стратегию
    // (версия 2 уже с is_transfer, но для теста backward compatibility
    // нужно создать файл версии 1 вручную)
    
    std::string filename = "old_format.bin";
    std::ofstream file(filename, std::ios::binary);
    uint32_t magic = 0x47524648;
    uint32_t version = 1;
    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Станции
    uint32_t station_count = old_graph.StationCount();
    file.write(reinterpret_cast<const char*>(&station_count), sizeof(station_count));
    for (const auto& [id, station] : old_graph.station_by_id) {
        file.write(reinterpret_cast<const char*>(&id), sizeof(id));
        uint32_t name_len = station.getName().size();
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(station.getName().c_str(), name_len);
        int line_num = station.getLineNo();
        file.write(reinterpret_cast<const char*>(&line_num), sizeof(line_num));
    }
    
    // Рёбра (без is_transfer)
    uint32_t edge_count = 0;
    for (const auto& [_, edges] : old_graph.adj_list) {
        edge_count += edges.size();
    }
    file.write(reinterpret_cast<const char*>(&edge_count), sizeof(edge_count));
    for (const auto& [from, edges] : old_graph.adj_list) {
        for (const Edge& e : edges) {
            file.write(reinterpret_cast<const char*>(&from), sizeof(from));
            file.write(reinterpret_cast<const char*>(&e.to), sizeof(e.to));
            file.write(reinterpret_cast<const char*>(&e.time), sizeof(e.time));
            file.write(reinterpret_cast<const char*>(&e.cost), sizeof(e.cost));
        }
    }
    file.close();
    
    // Загружаем старой версией (должна установить is_transfer = false)
    GraphSerializer serializer;
    serializer.setFormat("binary");
    auto loaded = serializer.load(filename);
    
    assert(loaded.StationCount() == 2);
    
    // Проверяем, что is_transfer = false для всех рёбер
    bool all_transfer_false = true;
    for (const auto& [from, edges] : loaded.adj_list) {
        for (const Edge& e : edges) {
            if (e.is_transfer) {
                all_transfer_false = false;
            }
        }
    }
    assert(all_transfer_false);
    
    std::cout << "✓ Backward compatibility works\n";
    
    fs::remove(filename);
}

void testPerformance() {
    printSeparator();
    std::cout << "Testing Performance (10000 edges)...\n";
    
    FastModificationGraph graph;
    
    // Добавляем 1000 станций
    for (int i = 0; i < 1000; ++i) {
        graph.AddStation(Station("Station_" + std::to_string(i), i % 10));
    }
    
    // Добавляем 10000 рёбер
    for (int i = 0; i < 10000; ++i) {
        size_t from = rand() % 1000;
        size_t to = rand() % 1000;
        int time = rand() % 30 + 1;
        int cost = rand() % 20 + 1;
        bool is_transfer = rand() % 2;
        graph.AddEdge(from, Edge(time, cost, to, is_transfer));
    }
    
    GraphSerializer serializer;
    
    // Бинарный формат
    auto start = std::chrono::high_resolution_clock::now();
    serializer.setFormat("binary");
    serializer.save(graph, "perf_bin.bin");
    auto mid = std::chrono::high_resolution_clock::now();
    auto loaded_bin = serializer.load("perf_bin.bin");
    auto end = std::chrono::high_resolution_clock::now();
    
    auto save_bin = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    auto load_bin = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    // JSON формат
    start = std::chrono::high_resolution_clock::now();
    serializer.setFormat("json");
    serializer.save(graph, "perf_json.json");
    mid = std::chrono::high_resolution_clock::now();
    auto loaded_json = serializer.load("perf_json.json");
    end = std::chrono::high_resolution_clock::now();
    
    auto save_json = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    auto load_json = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "Binary format:\n";
    std::cout << "  Save: " << save_bin.count() << " ms\n";
    std::cout << "  Load: " << load_bin.count() << " ms\n";
    std::cout << "  File: " << fs::file_size("perf_bin.bin") / 1024 << " KB\n\n";
    
    std::cout << "JSON format:\n";
    std::cout << "  Save: " << save_json.count() << " ms\n";
    std::cout << "  Load: " << load_json.count() << " ms\n";
    std::cout << "  File: " << fs::file_size("perf_json.json") / 1024 << " KB\n";
    
    assert(compareGraphs(graph, loaded_bin));
    assert(compareGraphs(graph, loaded_json));
    
    fs::remove("perf_bin.bin");
    fs::remove("perf_json.json");
    
    std::cout << "✓ Performance tests passed\n";
}

// ============= main =============
int main() {
    std::cout << "Graph and Serialization Tests\n";
    std::cout << "==============================\n";
    
    // Базовые тесты
    testAddStations();
    testAddEdges();
    
    // Тесты сериализации
    testBinarySerialization();
    testJSONSerialization();
    testTextSerialization();
    testAutoDetection();
    testBackwardCompatibility();
    
    // Тест производительности
    testPerformance();
    
    printSeparator();
    std::cout << "✓ All tests passed!\n";
    
    return 0;
}