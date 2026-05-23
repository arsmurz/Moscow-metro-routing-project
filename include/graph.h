template <typename VertexType, typename TimeUnit, typename MoneyUnit>
class Direction {  // то что будет лежать в соседях вершины
public:
    TimeUnit time;
    MoneyUnit cost;
    VertexType to;  // для экономии не храним from
};

template <template <typename...> typename NeighbourContainer, 
          typename VertexType, 
          typename TimeUnit, 
          typename MoneyUnit>
class Graph
{
private:
    std::unordered_map<VertexType, NeighbourContainer<Direction<VertexType, TimeUnit, MoneyUnit>>> AdjList;
    
public:
    NeighbourContainer<Direction<VertexType, TimeUnit, MoneyUnit>> GetNeighbours(VertexType vertex);
    virtual std::pair<TimeUnit, MoneyUnit> GetDistance(VertexType from, VertexType to);    
    Graph(std::istream& in_stream);    
    template <typename SourceContainer>
    Graph(SourceContainer container);
    void Save();
    virtual ~Graph();
};

template <typename VertexType, typename TimeUnit, typename MoneyUnit>
class FastModificationGraph : public Graph<std::set, VertexType, TimeUnit, MoneyUnit> {
public:
    std::pair<TimeUnit, MoneyUnit> GetDistance(VertexType from, VertexType to) override;
    void AddVertex(VertexType vertex);
    void AddEdge(VertexType from, VertexType to);
    void DeleteVertex(VertexType vertex);
    void DeleteEdge(VertexType from, VertexType to);
};