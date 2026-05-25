#include "graph.h"

size_t Station::id_counter = 0;

Station::Station(std::string name, int line_num) 
    : name(std::move(name)), line_num(line_num), id(id_counter++) {}

Station::Station(std::string name, int line_num, size_t id) 
    : name(std::move(name)), line_num(line_num), id(id) {
    if (id >= id_counter) id_counter = id + 1;
}

Edge::Edge(int time, int cost, size_t to) 
    : time(time), cost(cost), to(to) {}

template <template <typename...> typename NeighbourContainer>
Graph<NeighbourContainer>::Graph(std::istream& in_stream) {
    // TODO
}

template <template <typename...> typename NeighbourContainer>
template <typename SourceContainer>
Graph<NeighbourContainer>::Graph(SourceContainer source_adj) {
    // TODO
}

template <template <typename...> typename NeighbourContainer>
void Graph<NeighbourContainer>::Save() {
    // TODO
}

template <template <typename...> typename NeighbourContainer>
Graph<NeighbourContainer>::~Graph() {
    // TODO
}

void FastModificationGraph::AddStation(Station new_station) {
    stations.insert(new_station);
    Station_by_id[new_station.getId()] = new_station;
}

void FastModificationGraph::AddEdge(size_t from_id, Edge dir) {
    adj_list[from_id].insert(dir);
}

void FastModificationGraph::DeleteStation(size_t station_id) {
    for (auto &dir : adj_list[station_id]) {
        DeleteEdge(station_id, dir);
    }
    stations.erase(Station_by_id[station_id]);
    Station_by_id.erase(station_id);
}

void FastModificationGraph::DeleteEdge(size_t from_id, Edge dir) {
    adj_list[from_id].erase(dir);
}