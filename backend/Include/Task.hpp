#pragma once

#include <string>
#include <vector>

struct Task
{
    int id_;
    std::string description_;
    int duration_;
    std::vector<int> predecessors_;
};

struct TaskInfo
{
    int earliestStart_ = 0;
    int earliestFinish_ = 0;
    int latestStart_ = 0;
    int latestFinish_ = 0;
};