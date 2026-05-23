
template <typename VertexType, typename TimeUnit, typename MoneyUnit>
class Direction {  // то что будет лежать в соседях вершины
public:
  TimeUnit time;
  MoneyUnit cost;
  VertexType to;  // для экономии не храним from
};


template <typename NeighbourContainer, typename VertexType, typename TimeUnit, typename MoneyUnit>
class Graph
{
private:
    std::unordered_map<VertexType, <Direction<VertexType, TimeUnit, MoneyUnit>>> AdjList;
    // список смежности
public:
    Graph(std::ostream& os); // простое чтение откуда-нибудь
    template <typename SourceContainer>
    Graph(SourceContainer<std::pair<from, Direction>>);
    void Save(); // с аргументами еще придется определиться
    ~Graph();
};

template <typename VertexType, typename TimeUnit, typename MoneyUnit>
class FastModificationGraph : Graph<std::set, VertexType, TimeUnit, MoneyUnit>  {
  // все эти 4 штуки по запросу можно сделать не void
  void AddVertex(VertexType vertex);
  void AddEdge(VertexType from, VertexType to);
  void DeleteVertex(VertexType vertex);
  void AddEdge(VertexType from, VertexType to);

};
