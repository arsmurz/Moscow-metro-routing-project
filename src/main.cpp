#include "../include/navigator_facade.h"
#include "../include/console_ui.h"

int main() {
    // файл для загрузки по умолчаению
    NavigatorFacade facade("moscow_metro.json");
    ConsoleUI ui(facade);
    ui.run();
    return 0;
}