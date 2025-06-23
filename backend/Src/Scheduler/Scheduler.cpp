#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/graph/properties.hpp>

#include "Scheduler.hpp"

Scheduler::Scheduler() {}

void Scheduler::addTask(std::unique_ptr<Task> task) {
    taskMap_[task->id_] = task.get();
    taskInfoMap_[task->id_] = TaskInfo();
    tasks_.emplace_back(std::move(task));
}

void Scheduler::buildDAG() {
    Vertex vtx = 0;
    for (const auto& task : tasks_) {
        taskToVertex_[task->id_] = vtx;
        vertexToTask_[vtx] = task->id_;
        ++vtx;
    }

    graph_ = Graph(tasks_.size());

    for (const auto& task : tasks_) {
        Vertex u = taskToVertex_[task->id_];
        for (int pred : task->predecessors_) {
            if (taskToVertex_.find(pred) == taskToVertex_.end()) {
                throw std::runtime_error("Predecessor " + std::to_string(pred) + " not found");
            }
            Vertex v = taskToVertex_[pred];
            boost::add_edge(v, u, taskMap_[pred]->duration_, graph_);
        }
    }
}

void Scheduler::printDAG() const {
    std::cout << "\n--- DAG Structure ---\n";
    for (const auto& [id, task] : taskMap_) {
        Vertex vertex = taskToVertex_.at(id);
        std::cout << "Task " << id << " (" << task->description_ << "):\n";

        // Outgoing edges
        std::cout << "  Outgoing to: ";
        bool hasOut = false;
        for (auto edge : boost::make_iterator_range(boost::out_edges(vertex, graph_))) {
            Vertex to = boost::target(edge, graph_);
            int toId = vertexToTask_.at(to);
            int weight = boost::get(boost::edge_weight, graph_, edge);
            std::cout << toId << " (weight=" << weight << ") ";
            hasOut = true;
        }
        if (!hasOut) std::cout << "None";
        std::cout << "\n";

        // Incoming edges
        std::cout << "  Incoming from: ";
        bool hasIn = false;
        for (auto edge : boost::make_iterator_range(boost::in_edges(vertex, graph_))) {
            Vertex from = boost::source(edge, graph_);
            int fromId = vertexToTask_.at(from);
            int weight = boost::get(boost::edge_weight, graph_, edge);
            std::cout << fromId << " (weight=" << weight << ") ";
            hasIn = true;
        }
        if (!hasIn) std::cout << "None";
        std::cout << "\n";
    }
}

void Scheduler::forwardPass() {
    std::vector<Vertex> topOrder;
    boost::topological_sort(graph_, std::back_inserter(topOrder));

    // Initialize earliest start times
    for (const auto& task : tasks_) {
        taskInfoMap_[task->id_].earliestStart_ = 0;
    }

    for (Vertex u : boost::adaptors::reverse(topOrder)) {
        int fromId = vertexToTask_[u];
        for (auto edge : boost::make_iterator_range(boost::out_edges(u, graph_))) {
            Vertex v = boost::target(edge, graph_);
            int toId = vertexToTask_[v];
            int duration = boost::get(boost::edge_weight, graph_, edge);
            taskInfoMap_[toId].earliestStart_ = std::max(
                taskInfoMap_[toId].earliestStart_,
                taskInfoMap_[fromId].earliestStart_ + duration
            );
        }
    }

    for (const auto& [id, task] : taskMap_) {
        taskInfoMap_[id].earliestFinish_ = taskInfoMap_[id].earliestStart_ + task->duration_;
    }
}

void Scheduler::backwardPass() {
    // Find project duration
    int projectDuration = 0;
    for (const auto& [id, info] : taskInfoMap_) {
        projectDuration = std::max(projectDuration, info.earliestFinish_);
    }

    // Initialize latest finish times
    for (auto& [id, info] : taskInfoMap_) {
        info.latestFinish_ = projectDuration;
    }

    // Get topological order
    std::vector<Vertex> topOrder;
    boost::topological_sort(graph_, std::back_inserter(topOrder));

    // Process in topological order (sink to source)
    for (Vertex u : topOrder) {
        int toId = vertexToTask_[u];
        for (auto edge : boost::make_iterator_range(boost::in_edges(u, graph_))) {
            Vertex pred = boost::source(edge, graph_);
            int fromId = vertexToTask_[pred];
            taskInfoMap_[fromId].latestFinish_ = std::min(
                taskInfoMap_[fromId].latestFinish_,
                taskInfoMap_[toId].latestFinish_ - taskMap_[toId]->duration_
            );
        }
    }

    // Compute latest start times
    for (const auto& [id, task] : taskMap_) {
        taskInfoMap_[id].latestStart_ = taskInfoMap_[id].latestFinish_ - task->duration_;
    }
}

void Scheduler::computeCriticalPath() {
    forwardPass();
    backwardPass();
}

void Scheduler::printSchedule() {
    std::cout << std::left << std::setw(10) << "TaskID"
              << std::setw(25) << "Description"
              << std::setw(10) << "Duration"
              << std::setw(15) << "ES"
              << std::setw(15) << "EF"
              << std::setw(15) << "LS"
              << std::setw(15) << "LF"
              << std::setw(10) << "Slack" << '\n';
    std::cout << std::string(110, '-') << '\n';

    for (const auto& task : tasks_) {
        int id = task->id_;
        const auto& info = taskInfoMap_[id];
        int slack = info.latestStart_ - info.earliestStart_;
        std::cout << std::setw(10) << id
                  << std::setw(25) << task->description_
                  << std::setw(10) << task->duration_
                  << std::setw(15) << info.earliestStart_
                  << std::setw(15) << info.earliestFinish_
                  << std::setw(15) << info.latestStart_
                  << std::setw(15) << info.latestFinish_
                  << std::setw(10) << slack << '\n';
    }
}