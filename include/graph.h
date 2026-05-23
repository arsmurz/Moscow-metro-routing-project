#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <unordered_map>

struct Vertex {
    static size_t id_counter;
    std::string name;
    int line_num;
    size_t id;
    Vertex(std::string name, int line_num) {
        name= name;
        line_num=line_num;
        id = id_counter++;
    }
};

struct TimeUnit
{
    double val;
    char measure_unit_id;
};

struct MoneyUnit
{
    double val;
    char currency_id;
};

using TimeUnit = double;
using MoneyUnit = double;

class Direction {  // то что будет лежать в соседях вершины
public:
    TimeUnit time;
    MoneyUnit cost;
    size_t to; // для экономии не храним from
    size_t id;
};

template <template <typename...> typename NeighbourContainer>
class Graph
{
private:
    std::list<Direction> edges;
    std::list<Vertex> vertexes;
    std::unordered_map<size_t, NeighbourContainer<Direction*>> AdjList;
    std::unordered_map<size_t, Vertex*> vertex_by_id;
    std::unordered_map<size_t, Direction*> direction_by_id;
public:
    int32_t GetLineNum(size_t vertex_id) { return vertex.line_num; }
    std::string GetName(size_t vertex_id) { return vertex_by_id[Vertex_id].name; }
    NeighbourContainer<Direction*> GetNeighbours(size_t vertex_id) {
        return AdjList[vertex_id];
    }
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
    // все 4 можно сделать не void
    void AddVertex(VertexType vertex);
    void AddEdge(VertexType from, VertexType to);
    void DeleteVertex(VertexType vertex);
    void DeleteEdge(VertexType from, VertexType to);
};