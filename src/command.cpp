#include "../include/command.h"
#include <iostream>

FindRouteCommand::FindRouteCommand(NavigatorFacade& f, std::string s, std::string e)
    : facade(f), start(std::move(s)), end(std::move(e)) {}

void FindRouteCommand::print_route(const RouteResult& route, const std::string& title) const {
    std::cout << title << "\n";

    if (!route.success or route.lines.empty()) {
        std::cout << "путь не найден\n\n";
        return;
    }

    for (size_t i = 0; i < route.lines.size(); ++i) {
        if (i > 0) {
            std::cout << "  пересадка\n";
        }

        for (const auto& station : route.lines[i]) {
            std::cout << "  - " << station.name << "\n";
        }
    }
    std::cout << "\n";
}

void FindRouteCommand::execute() {
    AllRoutesResult routes = facade.find_all_routes(start, end);

    print_route(routes.fastest, "быстрый:");
    print_route(routes.cheapest, "дешевый:");
    print_route(routes.min_transfers, "мин пересадок:");
}