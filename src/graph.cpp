#include "graph.h"

size_t Station::id_counter = 0;

Station::Station(std::string name, int line_num) 
    : name(std::move(name)), line_num(line_num), id(id_counter++) {}

Station::Station(std::string name, int line_num, size_t id) 
    : name(std::move(name)), line_num(line_num), id(id) {
    if (id >= id_counter) id_counter = id + 1;
}

Edge::Edge(int time, int cost, size_t to, bool is_transfer) 
    : time(time), cost(cost), to(to), is_transfer(is_transfer) {}


void FastModificationGraph::AddStation(Station new_station) {
    if (name_to_id.find(new_station.getName()) != name_to_id.end()) {
        throw DuplicateStationException(new_station.getName());  // ← добавить
    }
    stations.insert(new_station);
    station_by_id[new_station.getId()] = new_station;
    name_to_id[new_station.getName()] = new_station.getId();
}

void FastModificationGraph::AddEdge(size_t from_id, Edge dir) {
        if (station_by_id.find(from_id) == station_by_id.end()) {
        throw StationNotFoundException(from_id);
    }
    if (station_by_id.find(dir.to) == station_by_id.end()) {
        throw StationNotFoundException(dir.to);
    }
    adj_list[from_id].insert(dir);
}

void FastModificationGraph::DeleteStation(size_t station_id) {
    if (station_by_id.find(station_id) == station_by_id.end()) {
        throw StationNotFoundException(station_id);
    }
    auto it = adj_list.find(station_id);
    if (it != adj_list.end()) {
        std::vector<Edge> edges_to_delete(it->second.begin(), it->second.end());
        for (const Edge& dir : edges_to_delete) {
            DeleteEdge(station_id, dir);
        }
    }
    
    // удаляем станцию
    auto station_it = station_by_id.find(station_id);
    if (station_it != station_by_id.end()) {
        const Station& dead_station = station_it->second;
        name_to_id.erase(dead_station.getName());
        stations.erase(dead_station);
        station_by_id.erase(station_id);
    }
}

void FastModificationGraph::DeleteEdge(size_t from_id, Edge dir) {
    auto it = adj_list.find(from_id);
    if (it == adj_list.end()) {
        throw EdgeNotFoundException(from_id, dir.to);  // ← добавить
    }
    if (it->second.find(dir) == it->second.end()) {
        throw EdgeNotFoundException(from_id, dir.to);  // ← добавить
    }
    it->second.erase(dir);
}