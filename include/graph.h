#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <optional>
#include <istream>

struct Vertex {
private:
    static size_t id_counter;
    std::string name;
    int line_num;
    size_t id;    
public:
    std::string getName() const { return name; }
    size_t getId() const { return id; }
    int getLineNo() const { return line_num; }
    
    Vertex(std::string name, int line_num);
    Vertex(std::string name, int line_num, size_t id);
};

class Direction {
public:
    int time;
    int cost;
    size_t to;
    Direction(int time, int cost, size_t to);
};

template <template <typename...> typename NeighbourContainer>
class Graph
{
protected:
    std::unordered_set<Direction> edges;
    std::unordered_set<Vertex> vertexes;
    std::unordered_map<size_t, NeighbourContainer<Direction>> adj_list;
    std::unordered_map<size_t, Vertex> vertex_by_id;
public:
    size_t vertexCount() const { return vertexes.size(); }
    int32_t getLineNum(size_t vertex_id) const { return vertex_by_id.at(vertex_id).getLineNo(); }
    std::string getName(size_t vertex_id) const { return vertex_by_id.at(vertex_id).getName(); }
    const NeighbourContainer<Vertex>& getVertexes() { return vertexes; }
    const NeighbourContainer<Direction>& getEdges(size_t vertex_id) {
        return adj_list[vertex_id];
    }
    Graph(std::istream& in_stream);
    template <typename SourceContainer>
    Graph(SourceContainer source);
    void Save();
    virtual ~Graph();
};

class FastModificationGraph : public Graph<std::unordered_set> {
public:
    void AddVertex(Vertex new_vertex);
    void AddEdge(size_t from, Direction dir);
    void DeleteVertex(size_t vertex);
    void DeleteEdge(size_t from, Direction dir);
};