#pragma once
#include "../include/routing.h"
#include <queue>
#include <vector>

template<typename Weight>
class RoutFinder{
  std::priority_queue<std::pair<int, Weight>, std::vector<std::pair<int, Weight>,
                        [](const P& left, const P& right) {
                            return left > right;
                        }
  std::shared_ptr<Graph> graph;
  std::vecotr<double> distance;
  int from;
  int to;
  void relaxation(int id) {
    std::vector<Direction> neighbours = graph->getAdj(id); // здесь должно быть получение вектора из соседей, todo
    for (auto edge : neighbours) {
        
    }
  }
  public:
    RouteFinder(std::shared_ptr<Graph> graph, int from, int to): graph(graph), from(from), to(to) {
        distance.resize(graph->GetStations(), cInf);
        distance[from] = 0;
    }
    std::vector<int> GetPath() {}
};
