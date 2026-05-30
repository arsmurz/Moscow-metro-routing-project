#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <optional>
#include <istream>
#include <generator>
#include <ranges>
#include <functional>

struct Station {
private:
    static size_t id_counter;
    std::string name;
    int line_num;
    size_t id;
public:
    std::string getName() const { return name; }
    size_t getId() const { return id; }
    int getLineNo() const { return line_num; }
    
    Station(std::string name="", int line_num=0);
    Station(std::string name, int line_num, size_t id);
};

class Edge {
public:
    int time;
    int cost;
    size_t to;
    bool is_transfer = 0;
    Edge(int time, int cost, size_t to, bool is_transfer);
};


namespace std {
    template<>
    struct hash<Station> {
        size_t operator()(const Station& s) const {
            // Используем id для хеширования (он уникален)
            return hash<size_t>()(s.getId());
        }
    };
    
    template<>
    struct hash<Edge> {
        size_t operator()(const Edge& e) const {
            // Комбинируем все поля, которые делают ребро уникальным
            size_t h1 = hash<size_t>()(e.to);
            size_t h2 = hash<int>()(e.time);
            size_t h3 = hash<int>()(e.cost);
            size_t h4 = hash<bool>()(e.is_transfer);
            // XOR комбинация (простой и эффективный способ)
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };
    
    template<>
    struct equal_to<Edge> {
        bool operator()(const Edge& lhs, const Edge& rhs) const {
            return lhs.to == rhs.to && 
                   lhs.time == rhs.time && 
                   lhs.cost == rhs.cost &&
                   lhs.is_transfer == rhs.is_transfer;
        }
    };
}

// Добавьте после определения классов Station и Edge, но до хеш-функций

inline bool operator==(const Station& lhs, const Station& rhs) {
    return lhs.getId() == rhs.getId() && 
           lhs.getName() == rhs.getName() && 
           lhs.getLineNo() == rhs.getLineNo();
}

inline bool operator==(const Edge& lhs, const Edge& rhs) {
    return lhs.to == rhs.to && 
           lhs.time == rhs.time && 
           lhs.cost == rhs.cost &&
           lhs.is_transfer == rhs.is_transfer;
}

class IGraph {
public:
  virtual void forEachEdge(size_t vertexId, 
                             std::function<bool(const Edge&)> visitor) const = 0;
  virtual void forEachVertex(std::function<bool(const Station&)> visitor) const = 0;
  virtual size_t StationCount() const = 0;
  virtual std::string getName(size_t vertex_id) const = 0;
  virtual size_t getId(std::string name) const = 0;
};

template <template <typename...> typename NeighbourContainer>
class Graph : public IGraph {
//protected:
public:
    std::unordered_set<Edge> edges;
    std::unordered_set<Station> stations;
    std::unordered_map<std::string, size_t> name_to_id;
    std::unordered_map<size_t, NeighbourContainer<Edge>> adj_list;
    std::unordered_map<size_t, Station> station_by_id;
// public:

    void forEachEdge(size_t vertexId, 
                    std::function<bool(const Edge&)> visitor) const override {
        auto it = adj_list.find(vertexId);
        if (it != adj_list.end()) {
            for (const Edge& edge : it->second) {
                if (!visitor(edge)) break;
            }
        }
    }

    void forEachVertex(std::function<bool(const Station&)> visitor) const override {
        for (const auto& [id, station] : station_by_id) {
            if (!visitor(station)) break;
        }
    }


    std::string getStationName(size_t station_id) const { return station_by_id.at(station_id).getName(); }
    size_t StationCount() const { return stations.size(); }
    virtual size_t getId(std::string name) const override { return name_to_id.at(name); }
    int32_t getLineNum(size_t Station_id) const { return station_by_id.at(Station_id).getLineNo(); }
    std::string getName(size_t Station_id) const { return station_by_id.at(Station_id).getName(); }
    const NeighbourContainer<Station>& getstations() { return stations; }
    const NeighbourContainer<Edge>& getEdges(size_t station_id) {
        return adj_list[station_id];
    }
    void Save();
    Graph() = default;
    virtual ~Graph() = default;
};

class FastModificationGraph : public Graph<std::unordered_set> {
public:
    FastModificationGraph() = default;
    ~FastModificationGraph() = default;
    void AddStation(Station new_Station);
    void AddEdge(size_t from, Edge dir);
    void DeleteStation(size_t station_id);
    void DeleteEdge(size_t from, Edge dir);
};