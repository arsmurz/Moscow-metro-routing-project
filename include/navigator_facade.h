#pragma once

#include <string>
#include <vector>
#include <future>
#include <memory>
#include "graph.h"
#include "routing.h"

struct RouteResult {
    std::vector<std::vector<Station>> lines;
    bool success = false;
};

struct AllRoutesResult {
    RouteResult fastest;
    RouteResult cheapest;
    RouteResult min_transfers;
};

class NavigatorFacade {
private:
    std::shared_ptr<FastModificationGraph> graph;
    RouteResult group_by_lines(const std::vector<Station>& raw_path) const;

public:
    NavigatorFacade(const std::string& map_filename);
    AllRoutesResult find_all_routes(const std::string& start_name, const std::string& end_name);
};