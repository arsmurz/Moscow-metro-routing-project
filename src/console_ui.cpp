#include "../include/console_ui.h"
#include "../include/command.h"
#include <iostream>
#include <string>
#include <cstdlib>

ConsoleUI::ConsoleUI(NavigatorFacade& f) : facade(f) {}

void ConsoleUI::run() {
    int choice = 0;
    while (choice != 3) {
        std::cout << "1 найти 2 очистить 3 выход ввод: ";
        std::cin >> choice;

        if (choice == 1) {
            std::string s, e;
            std::cout << "откуда: ";
            std::cin >> s;
            std::cout << "куда: ";
            std::cin >> e;

            FindRouteCommand cmd(facade, s, e);
            cmd.execute();
        } else if (choice == 2) {
#ifdef _WIN32
            std::system("cls");
#else
            std::system("clear");
#endif
        }
    }
}