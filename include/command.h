#pragma once
#include "navigator_facade.h"
#include <string>

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
};

class FindRouteCommand final : public ICommand {
    NavigatorFacade& facade;
    std::string start;
    std::string end;

    // вывод
    void print_route(const RouteResult& route, const std::string& title) const;

public:
    FindRouteCommand(NavigatorFacade& f, std::string s, std::string e);
    void execute() override;
};