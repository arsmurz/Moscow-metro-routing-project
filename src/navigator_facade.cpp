#include "navigator_facade.h"

bool NavigatorFacade::addStation(const std::string &name, int line) {
  try {
    graph_->AddStation(Station(name, line));
    return true;
  } catch (const DuplicateStationException &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return false;
  }
}

bool NavigatorFacade::addEdge(const std::string &from_name,
                              const std::string &to_name, int time, int cost) {
  try {
    size_t from_id = graph_->getId(from_name);
    size_t to_id = graph_->getId(to_name);
    graph_->AddEdge(from_id, Edge(time, cost, to_id,
                                  (graph_->getLineNum(from_id) ==
                                   graph_->getLineNum(to_id))));
    return true;
  } catch (const StationNotFoundException &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return false;
  }
}

bool NavigatorFacade::saveGraph(const std::string &filename) const {
  try {
    GraphSerializer serializer;
    serializer.setFormatByExtension(filename);
    return serializer.save(*graph_, filename);
  } catch (const FileNotFoundException &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "Ошибка сохранения: " << e.what() << std::endl;
    return false;
  }
}

bool NavigatorFacade::loadGraph(const std::string &filename) {
  try {
    GraphSerializer serializer;
    serializer.setFormatByExtension(filename);
    FastModificationGraph loaded = serializer.load(filename);
    graph_ = std::make_shared<FastModificationGraph>(std::move(loaded));
    return true;
  } catch (const FileNotFoundException &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return false;
  } catch (const InvalidFormatException &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "Ошибка загрузки: " << e.what() << std::endl;
    return false;
  }
}

RouteResult
NavigatorFacade::group_by_lines(const std::vector<Station> &raw_path) const {
  RouteResult result;
  if (raw_path.empty())
    return result;

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

std::optional<AllRoutesResult>
NavigatorFacade::find_all_routes(const std::string &start_name,
                                 const std::string &end_name) {
  try {
    size_t start_id = graph_->getId(start_name);
    size_t end_id = graph_->getId(end_name);

    // лямбда для поиска со стратегией
    auto run_search = [&](auto strategy_tag) -> RouteResult {
      using Strategy = decltype(strategy_tag);
      PathFinder<Strategy> finder(graph_);
      PathResult raw = finder.findShortestPath(start_id, end_id);

      std::vector<Station> station_path;
      if (raw.found) {
        station_path.reserve(raw.path.size());
        for (size_t id : raw.path) {
          station_path.push_back(graph_->getStationById(id));
        }
      }
      return group_by_lines(station_path);
    };

    // три поиска параллельно
    auto future_fast = std::async(
        std::launch::async, [&]() { return run_search(FastestStrategy{}); });

    auto future_cheap = std::async(
        std::launch::async, [&]() { return run_search(CheapestStrategy{}); });

    auto future_min = std::async(std::launch::async, [&]() {
      return run_search(MinTransfersStrategy{});
    });

    AllRoutesResult result;
    result.fastest = future_fast.get();
    result.cheapest = future_cheap.get();
    result.min_transfers = future_min.get();

    return result;

  } catch (const StationNotFoundException &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return std::nullopt;
  } catch (const std::exception &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
    return std::nullopt;
  }
}

NavigatorFacade::NavigatorFacade(const std::string &map_filename) {
  graph_ = std::make_shared<FastModificationGraph>();
  if (!map_filename.empty()) {
    loadGraph(map_filename);
  }
}
