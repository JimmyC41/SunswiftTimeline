#include <iostream>
#include "Scheduler.hpp"

int main() {
    Scheduler scheduler;
    scheduler.addTask(std::make_unique<Task>(1, "Task 1", 5, std::vector<int>{}));
    scheduler.addTask(std::make_unique<Task>(2, "Task 2", 3, std::vector<int>{1}));
    scheduler.addTask(std::make_unique<Task>(3, "Task 3", 4, std::vector<int>{1}));
    scheduler.addTask(std::make_unique<Task>(4, "Task 4", 2, std::vector<int>{2, 3}));
    scheduler.buildDAG();
    scheduler.printDAG();
    scheduler.computeCriticalPath();
    scheduler.printSchedule();
    return 0;
}