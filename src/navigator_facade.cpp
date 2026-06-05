#include "../include/navigator_facade.h"
#include "../include/graph_serializer.h"

NavigatorFacade::NavigatorFacade(const std::string& map_filename) {
    GraphSerializer serializer;
    serializer.setFormatByExtension(map_filename);
    FastModificationGraph loaded = serializer.load(map_filename);
    graph = std::make_shared<FastModificationGraph>(std::move(loaded));
}

RouteResult NavigatorFacade::group_by_lines(const std::vector<Station>& raw_path) const {
    RouteResult result;
    if (raw_path.empty()) return result;

    std::vector<Station> current_line;
    current_line.push_back(raw_path[0]);

    for (size_t i = 1; i < raw_path.size(); ++i) {
        if (raw_path[i - 1].getLineNo() == raw_path[i].getLineNo()) {
            current_line.push_back(raw_path[i]);
        } else {
            result.lines.push_back(current_line);
            current_line.clear();
            current_line.push_back(raw_path[i]);
        }
    }

    if (!current_line.empty()) {
        result.lines.push_back(current_line);
    }

    result.success = true;
    return result;
}

AllRoutesResult NavigatorFacade::find_all_routes(const std::string& start_name, const std::string& end_name) {
    AllRoutesResult final_res;

    size_t start_id = graph->getId(start_name);
    size_t end_id = graph->getId(end_name);

    auto future_fast = std::async(std::launch::async, [this, start_id, end_id]() {
        PathFinder<FastestStrategy> finder(graph);
        PathResult raw = finder.findShortestPath(start_id, end_id);

        std::vector<Station> station_path;
        if (raw.found) {
            for (size_t id : raw.path) {
                station_path.push_back(graph->station_by_id.at(id));
            }
        }
        return this->group_by_lines(station_path);
    });

    auto future_cheap = std::async(std::launch::async, [this, start_id, end_id]() {
        PathFinder<CheapestStrategy> finder(graph);
        PathResult raw = finder.findShortestPath(start_id, end_id);

        std::vector<Station> station_path;
        if (raw.found) {
            for (size_t id : raw.path) {
                station_path.push_back(graph->station_by_id.at(id));
            }
        }
        return this->group_by_lines(station_path);
    });

    auto future_transfers = std::async(std::launch::async, [this, start_id, end_id]() {
        PathFinder<MinTransfersStrategy> finder(graph);
        PathResult raw = finder.findShortestPath(start_id, end_id);

        std::vector<Station> station_path;
        if (raw.found) {
            for (size_t id : raw.path) {
                station_path.push_back(graph->station_by_id.at(id));
            }
        }
        return this->group_by_lines(station_path);
    });

    final_res.fastest = future_fast.get();
    final_res.cheapest = future_cheap.get();
    final_res.min_transfers = future_transfers.get();

    return final_res;
}