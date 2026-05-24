#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

struct Edge {
  int to;
  int time;
  int cost;
  bool isTransfer;
};

class IGraph {
public:
  virtual ~IGraph() = default;
  virtual const std::vector<Edge> &getEdges(int vertexId) const = 0;
  virtual size_t vertexCount() const = 0;
};

struct PathResult {
  int totalWeight = 0;
  std::vector<int> path;
  bool found = false;
};

//Самый быстрый путь
struct FastestStrategy {
  static int edgeWeight(const Edge &edge) { return edge.time; }
};

//Самый дешевый путь
struct CheapestStrategy {
  static int edgeWeight(const Edge &edge) { return edge.cost; }
};

//Минимум пересадок
struct MinTransfersStrategy {
  static int edgeWeight(const Edge &edge) { return edge.isTransfer ? 1 : 0; }
};

template <typename Strategy> class PathFinder {
public:
  explicit PathFinder(std::shared_ptr<IGraph> graph)
      : graph_(std::move(graph)) {}

  PathResult findShortestPath(int start, int finish) const {
    const int INF = 1e9;

    std::vector<int> dist(graph_->vertexCount(), INF);
    std::vector<int> parent(graph_->vertexCount(), -1);

    using QueueItem = std::pair<int, int>;

    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> pq;
    dist[start] = 0;
    pq.emplace(0, start);
    while (!pq.empty()) {
      auto [currentDist, vertex] = pq.top();
      pq.pop();

      if (currentDist > dist[vertex]) { continue; }
      if (vertex == finish) { break; }
      for (const auto &edge : graph_->getEdges(vertex)) {
        const int weight = Strategy::edgeWeight(edge);
        if (dist[vertex] + weight < dist[edge.to]) {
          dist[edge.to] = dist[vertex] + weight;
          parent[edge.to] = vertex;
          pq.emplace(dist[edge.to], edge.to);
        }
      }
    }
    PathResult result;
    if (dist[finish] == INF) {
      return result;
    }
    result.totalWeight = dist[finish];
    result.found = true;
    for (int v = finish; v != -1; v = parent[v]) {
      result.path.push_back(v);
    }
    std::reverse(result.path.begin(), result.path.end());
    return result;
  }
private:
  std::shared_ptr<IGraph> graph_;
};
