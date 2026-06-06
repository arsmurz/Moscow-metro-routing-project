#include "../include/command.h"
#include <iostream>

FindRouteCommand::FindRouteCommand(NavigatorFacade& f, std::string s, std::string e)
    : facade(f), start(std::move(s)), end(std::move(e)) {}

void FindRouteCommand::printRoute(const RouteResult& route, const char* title) const {
    std::cout << title;
    
    if (!route.success || route.lines.empty()) {
        std::cout << " не найден\n";
        return;
    }
    
    int stations = 0;
    for (const auto& line : route.lines) {
        if (!line.empty()) {
            std::cout << " (л." << line[0].getLineNo() << ")";
            for (const auto& s : line) {
                std::cout << " " << s.getName();
            }
            stations += line.size();
        }
    }
    std::cout << " [" << stations << " ст.]\n";
}

void FindRouteCommand::execute() {
    std::cout << "\nПоиск маршрута " << start << " -> " << end << "...\n";
    
    auto routes_opt = facade.find_all_routes(start, end);
    
    if (!routes_opt.has_value()) {
        std::cout << "Ошибка: станция не найдена\n";
        return;
    }
    
    const AllRoutesResult& routes = routes_opt.value();
    
    std::cout << "Быстрый:";
    printRoute(routes.fastest, "");
    
    std::cout << "Дешевый:";
    printRoute(routes.cheapest, "");
    
    std::cout << "Мин.пересадок:";
    printRoute(routes.min_transfers, "");
}