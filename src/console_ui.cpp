#include "../include/command.h"
#include "../include/console_ui.h"
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

ConsoleUI::ConsoleUI(NavigatorFacade &f) : facade(f) {}

void ConsoleUI::printHelp() const {
  std::cout << "\nДоступные команды:\n";
  std::cout << "  route <откуда> <куда>                    - найти маршрут\n";
  std::cout
      << "  add_station <имя> <линия>                - добавить станцию\n";
  std::cout << "  add_edge <откуда> <куда> <время> <цена>  - добавить ребро\n";
  std::cout
      << "  list_stations                            - показать все станции\n";
  std::cout
      << "  list_edges                               - показать все рёбра\n";
  std::cout << "  save <файл>                              - сохранить граф\n";
  std::cout << "  load <файл>                              - загрузить граф\n";
  std::cout << "  help                                     - справка\n";
  std::cout << "  exit                                     - выход\n";
}

void ConsoleUI::listStations() const {
  size_t count = facade.graph_->getStationCount();
  if (count == 0) {
    std::cout << "Нет станций\n";
    return;
  }

  std::cout << "\nСписок станций (" << count << "):\n";
  facade.graph_->forEachVertex([](const Station &s) {
    std::cout << "  " << s.getId() << ": " << s.getName() << " (линия "
              << s.getLineNo() << ")\n";
    return true;
  });
}

void ConsoleUI::listEdges() const {
  std::cout << "\nСписок рёбер:\n";

  bool has_edges = false;
  facade.graph_->forEachVertex([&](const Station &station) {
    size_t from = station.getId();
    facade.graph_->forEachEdge(from, [&](const Edge &e) {
      std::cout << "  " << facade.graph_->getName(from) << "(" << from << ")"
                << " -> " << facade.graph_->getName(e.to) << "(" << e.to << ")"
                << " (время=" << e.time << ", цена=" << e.cost
                << ", пересадка=" << (e.is_transfer ? "да" : "нет") << ")\n";
      has_edges = true;
      return true;
    });
    return true;
  });

  if (!has_edges) {
    std::cout << "  Нет рёбер\n";
  }
}

void ConsoleUI::addStationCommand(const std::string &name, int line) {
  if (name.empty()) {
    std::cout << "Ошибка: имя станции не может быть пустым\n";
    return;
  }

  if (facade.addStation(name, line)) {
    std::cout << "Станция '" << name << "' добавлена (линия " << line << ")\n";
  } else {
    std::cout << "Ошибка: станция '" << name << "' уже существует\n";
  }
}

void ConsoleUI::addEdgeCommand(const std::string &from, const std::string &to,
                               int time, int cost) {
  if (from.empty() || to.empty()) {
    std::cout << "Ошибка: имена станций не могут быть пустыми\n";
    return;
  }

  if (time <= 0) {
    std::cout << "Ошибка: время должно быть положительным\n";
    return;
  }

  if (cost <= 0) {
    std::cout << "Ошибка: цена должна быть положительной\n";
    return;
  }
  if (facade.addEdge(from, to, time, cost)) {
    std::cout << "Ребро добавлено: " << from << " -> " << to
              << " (время=" << time << ", цена=" << cost << ")\n";
  } else {
    std::cout << "Ошибка: не удалось добавить ребро (проверьте существование "
                 "станций)\n";
  }
}

void ConsoleUI::parseCommand(const std::string &cmd) {
  std::istringstream iss(cmd);
  std::string command;
  iss >> command;

  if (command == "route") {
    std::string from, to;
    iss >> from >> to;
    if (from.empty() || to.empty()) {
      std::cout << "Использование: route <откуда> <куда>\n";
      return;
    }
    FindRouteCommand routeCmd(facade, from, to);
    routeCmd.execute();
  } else if (command == "add_station") {
    std::string name;
    int line;
    iss >> name >> line;
    if (name.empty()) {
      std::cout << "Использование: add_station <имя> <линия>\n";
      return;
    }
    addStationCommand(name, line);
  } else if (command == "add_edge") {
    std::string from, to;
    int time, cost;
    iss >> from >> to >> time >> cost;

    if (from.empty() || to.empty()) {
      std::cout << "Использование: add_edge <откуда> <куда> <время> <цена>\n";
      return;
    }
    addEdgeCommand(from, to, time, cost);
  } else if (command == "list_stations") {
    listStations();
  } else if (command == "list_edges") {
    listEdges();
  } else if (command == "save") {
    std::string filename;
    iss >> filename;
    if (filename.empty()) {
      std::cout << "Использование: save <файл>\n";
      return;
    }
    if (facade.saveGraph(filename)) {
      std::cout << "Граф сохранён в " << filename << "\n";
    } else {
      std::cout << "Ошибка при сохранении графа\n";
    }
  } else if (command == "load") {
    std::string filename;
    iss >> filename;
    if (filename.empty()) {
      std::cout << "Использование: load <файл>\n";
      return;
    }
    if (facade.loadGraph(filename)) {
      std::cout << "Граф загружен из " << filename << "\n";
      listStations();
    } else {
      std::cout << "Ошибка при загрузке графа\n";
    }
  } else if (command == "help") {
    printHelp();
  } else if (command == "exit" || command == "quit") {
    // в run() уже обрабатывается
  } else {
    std::cout << "Неизвестная команда. Введите 'help' для справки.\n";
  }
}

void ConsoleUI::run() {
  printHelp();

  std::string line;
  while (true) {
    std::cout << "\n> ";
    std::getline(std::cin, line);

    if (line == "exit" || line == "quit") {
      break;
    }

    if (!line.empty()) {
      parseCommand(line);
    }
  }
}