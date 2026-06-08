#include "graph.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <vector>

struct PathResult {
  int totalWeight = 0;
  std::vector<size_t> path;
  bool found = false;
};

// Самый быстрый путь
struct FastestStrategy {
  static int edgeWeight(const Edge &edge) { return edge.time; }
};

// Самый дешевый путь
struct CheapestStrategy {
  static int edgeWeight(const Edge &edge) { return edge.cost; }
};

// Минимум пересадок
struct MinTransfersStrategy {
  static int edgeWeight(const Edge &edge) { return edge.is_transfer ? 1 : 0; }
};

template <typename Strategy> class PathFinder {
public:
  explicit PathFinder(std::shared_ptr<IGraph> graph)
      : graph_(std::move(graph)) {}

  PathResult findShortestPath(size_t start, size_t finish) const {
    assert(graph_ != nullptr);
    const int INF = std::numeric_limits<int>::max();

    // map так как id могут быть рабросаны
    std::unordered_map<size_t, int> dist;
    std::unordered_map<size_t, size_t> parent;

    using QueueItem = std::pair<int, size_t>;
    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> pq;

    dist[start] = 0;
    pq.emplace(0, start);

    while (!pq.empty()) {
      auto [currentDist, vertex] = pq.top();
      pq.pop();

      if (currentDist > dist[vertex]) {
        continue;
      }
      if (vertex == finish) {
        break;
      }

      graph_->forEachEdge(vertex, [&](const Edge &edge) {
        const int weight = Strategy::edgeWeight(edge);
        assert(weight >= 0 && "Dijkstra requires non-negative weights");
        int newDist = dist[vertex] + weight;

        auto it = dist.find(edge.to);
        if (it == dist.end() || newDist < it->second) {
          dist[edge.to] = newDist;
          parent[edge.to] = vertex;
          pq.emplace(newDist, edge.to);
        }
        return true;
      });
    }

    PathResult result;
    auto it = dist.find(finish);
    if (it == dist.end()) {
      return result;
    }

    result.totalWeight = it->second;
    result.found = true;

    for (size_t v = finish; v != start; v = parent[v]) {
      result.path.push_back(v);
      if (parent.find(v) == parent.end())
        break;
    }
    result.path.push_back(start);
    std::reverse(result.path.begin(), result.path.end());

    return result;
  }

private:
  std::shared_ptr<IGraph> graph_;
};