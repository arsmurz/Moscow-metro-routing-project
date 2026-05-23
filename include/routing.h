#pragma once
#include <vector>
#include <memory.h>
#include "graph.h"

const double cInf = 1e18;

struct CostWeight{
    double weight;
    bool operator < (CostWeight right);
    bool operator > (CostWeight right);
    bool operator <= (CostWeight right);
    bool operator >= (CostWeight right);
    CostWeight operator += (CostWeight right);
    CostWeight(Direction dir);
    
};

class RoutFinderInerface{
  public:
    RoutFinderInerface(std::shared_ptr<Graph> graph, std::string mode): graph(graph){}
    virtual std::vector<Station> GetPath(Station start, Station finish);
    virtual ~RoutFinderInerface();
  private:
    std::unique_ptr<RoutFinder> finder;
};

template<typename Weight>
class RouteFinder{
  public:
    RoutFinder(std::shared_ptr<Graph> graph): graph(graph){
        distances.resize(graph->getVertexCount(), cInf);
    }
   

};
