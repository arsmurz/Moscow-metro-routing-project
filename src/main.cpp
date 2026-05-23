#include "../include/navigator_facade.h"
#include "../include/console_ui.h"

int main() {
    NavigatorFacade facade;
    ConsoleUI ui(facade);
    ui.run();
    return 0;
}