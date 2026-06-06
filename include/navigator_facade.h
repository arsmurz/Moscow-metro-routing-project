#pragma once

#include <string>
#include <vector>
#include <future>
#include <memory>
#include "graph.h"
#include "routing.h"
#include "exceptions.h"
#include "graph_serializer.h"

struct RouteResult {
    std::vector<std::vector<Station>> lines;  // список линий, каждая линия - список станций
    bool success = false;
    
    RouteResult() = default;
    explicit RouteResult(bool s) : success(s) {}
};

struct AllRoutesResult {
    RouteResult fastest;        // самый быстрый маршрут
    RouteResult cheapest;       // самый дешёвый маршрут
    RouteResult min_transfers;  // маршрут с минимумом пересадок
};

class NavigatorFacade {
public:
    NavigatorFacade(const std::string& map_filename);
    // ошибка обработана внутри
    bool addStation(const std::string& name, int line);
    bool addEdge(const std::string& from_name, const std::string& to_name, 
                 int time, int cost);
    bool saveGraph(const std::string& filename) const;
    bool loadGraph(const std::string& filename);
    
    std::optional<AllRoutesResult> find_all_routes(const std::string& start_name, 
                                                    const std::string& end_name);
    
    size_t getStationId(const std::string& name) const;
    Station getStation(size_t id) const;
    std::shared_ptr<FastModificationGraph> graph_;
private:
    RouteResult group_by_lines(const std::vector<Station>& raw_path) const;
};


