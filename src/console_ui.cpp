#include "../include/console_ui.h"
#include "../include/command.h"
#include <iostream>
#include <string>
#include <cstdlib>

ConsoleUI::ConsoleUI(NavigatorFacade& f) : facade(f) {}

void ConsoleUI::run() {
    int choice = 0;
    while (true) {
        std::cout << "1 найти\n2 очистить\n3 выход\nввод: ";
        if (!(std::cin >> choice)) break;

        if (choice == 1) {
            std::string s, e;
            std::cout << "откуда: ";
            std::cin >> s;
            std::cout << "куда: ";
            std::cin >> e;
            FindRouteCommand cmd(facade, s, e);
            cmd.execute();
        } else if (choice == 2) {
            std::system("cls");
        } else if (choice == 3) {
            break;
        }
    }
}