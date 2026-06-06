#pragma once
#include "navigator_facade.h"
#include <string>

class ConsoleUI {
  NavigatorFacade &facade;

  void printHelp() const;
  void parseCommand(const std::string &cmd);

  void listStations() const;
  void listEdges() const;
  void addStationCommand(const std::string &name, int line);
  void addEdgeCommand(const std::string &from, const std::string &to, int time,
                      int cost);

public:
  explicit ConsoleUI(NavigatorFacade &f);
  void run();
};