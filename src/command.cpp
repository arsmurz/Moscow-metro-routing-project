#include "../include/command.h"
#include <iostream>

FindRouteCommand::FindRouteCommand(NavigatorFacade& f, std::string s, std::string e)
    : facade(f), start(std::move(s)), end(std::move(e)) {}

void FindRouteCommand::print_route(const RouteResult& route, const std::string& title) const {
    std::cout << title;

    if (!route.success || route.lines.empty()) {
        std::cout << " маршрут не найден";
        return;
    }

    for (size_t i = 0; i < route.lines.size(); ++i) {
        if (i > 0) {
            std::cout << " пересадка";
        }
        for (const auto& station : route.lines[i]) {
            std::cout << " " << station.getName();
        }
    }
}

void FindRouteCommand::execute() {
    AllRoutesResult routes = facade.find_all_routes(start, end);

    print_route(routes.fastest, "быстрый:");
    std::cout << " ";
    print_route(routes.cheapest, "дешевый:");
    std::cout << " ";
    print_route(routes.min_transfers, "мин пересадок:");
}