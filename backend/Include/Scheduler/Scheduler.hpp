#pragma once

#include <vector>
#include <map>
#include <memory> // Added for std::unique_ptr
#include <boost/graph/adjacency_list.hpp>

#include "Task.hpp"

// Define Graph with bidirectionalS for in_edges support
using Graph = boost::adjacency_list<
    boost::vecS,          // OutEdgeListS: Store out-edges in a vector
    boost::vecS,          // VertexListS: Store vertices in a vector
    boost::bidirectionalS, // DirectedS: Bidirectional for in_edges and out_edges
    boost::no_property,   // Vertex properties (not needed)
    boost::property<boost::edge_weight_t, int> // Edge weight property
>;

// Define Vertex and Edge types after Graph
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
using Edge = boost::graph_traits<Graph>::edge_descriptor;

class Scheduler {
public:
    Scheduler();
    void addTask(std::unique_ptr<Task> task);
    void buildDAG();
    void printDAG() const;
    void computeCriticalPath();
    void printSchedule();

private:
    std::vector<std::unique_ptr<Task>> tasks_;
    std::map<int, Task*> taskMap_;
    std::map<int, TaskInfo> taskInfoMap_;
    Graph graph_;
    std::map<int, Vertex> taskToVertex_;
    std::map<Vertex, int> vertexToTask_;

    void forwardPass();
    void backwardPass();
};