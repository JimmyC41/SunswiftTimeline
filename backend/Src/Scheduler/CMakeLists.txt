add_library(SchedulerModule
    Scheduler.cpp
)

target_include_directories(SchedulerModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Include
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Include/Scheduler
)

find_package(Boost REQUIRED COMPONENTS graph)

target_link_libraries(SchedulerModule
    PUBLIC
        Boost::graph
)