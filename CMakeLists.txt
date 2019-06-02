cmake_minimum_required(VERSION 3.0)

project(dynamical)

include_directories("./src")
file(GLOB_RECURSE sources main.cpp src/*.cpp src/*.h)

add_executable(dynamical ${sources})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(dynamical SDL2::SDL2 SDL2::SDL2main)
