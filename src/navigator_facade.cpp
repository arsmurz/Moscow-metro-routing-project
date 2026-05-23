#include "../include/navigator_facade.h"

RouteResult NavigatorFacade::group_by_lines(const std::vector<Station>& raw_path) const {
    RouteResult result;

    if (raw_path.empty()) {
        return result;
    }

    std::vector<Station> current_line;
    current_line.push_back(raw_path[0]);

    for (size_t i = 1; i < raw_path.size(); ++i) {

        // вызов функции Андрея
        // если возвращает true то пересадка и начинаем новую линию
        if (is_transfer(raw_path[i - 1], raw_path[i])) {
            result.lines.push_back(current_line);
            current_line.clear();
        }
        current_line.push_back(raw_path[i]);
    }

    if (!current_line.empty()) {
        result.lines.push_back(current_line);
    }

    result.success = true;
    return result;
}


AllRoutesResult NavigatorFacade::find_all_routes(const std::string& start, const std::string& end) {
    AllRoutesResult final_res;

    // 1 поток
    auto future_fast = std::async(std::launch::async, [this, &start, &end]() {
        // быстрый путь
        std::vector<Station> raw = get_fastest_path(start, end);
        return this->group_by_lines(raw);
    });

    // 1 поток
    auto future_cheap = std::async(std::launch::async, [this, &start, &end]() {
        // дешевый путь
        std::vector<Station> raw = get_cheapest_path(start, end);
        return this->group_by_lines(raw);
    });

    // 3 поток
    auto future_transfers = std::async(std::launch::async, [this, &start, &end]() {
        // минимальные пересадки
        std::vector<Station> raw = get_min_transfers_path(start, end);
        return this->group_by_lines(raw);
    });

    final_res.fastest = future_fast.get();
    final_res.cheapest = future_cheap.get();
    final_res.min_transfers = future_transfers.get();

    return final_res;
}