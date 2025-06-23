cmake_minimum_required(VERSION 4.0.2)
set(CMAKE_CXX_STANDARD 23)

project(Sunswift-Timeline)

add_subdirectory(Src/Scheduler)

add_executable(main
    main.cpp
)

target_include_directories(main
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Include
        ${CMAKE_SOURCE_DIR}/Include/Scheduler
)

target_link_libraries(main
    PRIVATE
        SchedulerModule
)