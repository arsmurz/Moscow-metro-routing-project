#include "graph.h"

size_t Vertex::id_counter = 0;

Vertex::Vertex(std::string name, int line_num) 
    : name(std::move(name)), line_num(line_num), id(id_counter++) {}

Vertex::Vertex(std::string name, int line_num, size_t id) 
    : name(std::move(name)), line_num(line_num), id(id) {
    if (id >= id_counter) id_counter = id + 1;
}

Direction::Direction(int time, int cost, size_t to) 
    : time(time), cost(cost), to(to) {}

template <template <typename...> typename NeighbourContainer>
Graph<NeighbourContainer>::Graph(std::istream& in_stream) {
    // TODO
}

template <template <typename...> typename NeighbourContainer>
template <typename SourceContainer>
Graph<NeighbourContainer>::Graph(SourceContainer source_adj) {
    // TODO
}

template <template <typename...> typename NeighbourContainer>
void Graph<NeighbourContainer>::Save() {
    // TODO
}

template <template <typename...> typename NeighbourContainer>
Graph<NeighbourContainer>::~Graph() {
    // TODO
}

void FastModificationGraph::AddVertex(Vertex new_vertex) {
    vertexes.insert(new_vertex);
    vertex_by_id[new_vertex.getId()] = new_vertex;
}

void FastModificationGraph::AddEdge(size_t from_id, Direction dir) {
    adj_list[from_id].insert(dir);
}

void FastModificationGraph::DeleteVertex(size_t vertex_id) {
    for (auto &dir : adj_list[vertex_id]) {
        DeleteEdge(vertex_id, dir);
    }
    vertexes.erase(vertex_by_id[vertex_id]);
    vertex_by_id.erase(vertex_id);
}

void FastModificationGraph::DeleteEdge(size_t from_id, Direction dir) {
    adj_list[from_id].erase(dir);
}