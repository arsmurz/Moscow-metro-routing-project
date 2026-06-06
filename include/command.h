#pragma once
#include "navigator_facade.h"
#include <string>

class FindRouteCommand {
    NavigatorFacade& facade;
    std::string start;
    std::string end;
    
    void printRoute(const RouteResult& route, const char* title) const;
    
public:
    FindRouteCommand(NavigatorFacade& f, std::string s, std::string e);
    void execute();
};