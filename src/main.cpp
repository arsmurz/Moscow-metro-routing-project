#include "../include/navigator_facade.h"
#include "../include/console_ui.h"

int main() {
    // Впиши сюда файл, который вы генерируете для карты
    NavigatorFacade facade("moscow_metro.json");
    ConsoleUI ui(facade);
    ui.run();
    return 0;
}