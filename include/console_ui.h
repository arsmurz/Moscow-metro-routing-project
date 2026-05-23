#pragma once
#include "navigator_facade.h"

class ConsoleUI {
    NavigatorFacade& facade;
public:
    explicit ConsoleUI(NavigatorFacade& f);
    void run();
};