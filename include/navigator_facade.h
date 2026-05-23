#pragma once

#include <string>
#include <vector>
#include <future>

// инклуд хэдеров с обещанной функцией и классом Station

struct RouteResult {
    // каждый внутренний вектор это станции на одной линии
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
    // метод разделения на линии
    RouteResult group_by_lines(const std::vector<Station>& raw_path) const;

public:
    NavigatorFacade() = default;

    // главный метод
    AllRoutesResult find_all_routes(const std::string& start, const std::string& end);
};