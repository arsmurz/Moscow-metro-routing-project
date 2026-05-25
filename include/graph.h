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
    
    Station(std::string name, int line_num);
    Station(std::string name, int line_num, size_t id);
};

class Edge {
public:
    int time;
    int cost;
    size_t to;
    Edge(int time, int cost, size_t to);
};

template <template <typename...> typename NeighbourContainer>
class Graph
{
protected:
    std::unordered_set<Edge> edges;
    std::unordered_set<Station> stations;
    std::unordered_map<size_t, NeighbourContainer<Edge>> adj_list;
    std::unordered_map<size_t, Station> Station_by_id;
public:
    size_t StationCount() const { return stations.size(); }
    int32_t getLineNum(size_t Station_id) const { return Station_by_id.at(Station_id).getLineNo(); }
    std::string getName(size_t Station_id) const { return Station_by_id.at(Station_id).getName(); }
    const NeighbourContainer<Station>& getstations() { return stations; }
    const NeighbourContainer<Edge>& getEdges(size_t station_id) {
        return adj_list[station_id];
    }
    Graph(std::istream& in_stream);
    template <typename SourceContainer>
    Graph(SourceContainer source);
    void Save();
    virtual ~Graph();
};

class FastModificationGraph : public Graph<std::unordered_set> {
public:
    void AddStation(Station new_Station);
    void AddEdge(size_t from, Edge dir);
    void DeleteStation(size_t station_id);
    void DeleteEdge(size_t from, Edge dir);
};