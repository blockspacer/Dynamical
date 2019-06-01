cmake_minimum_required(VERSION 3.0)

project(dynamical)


include_directories("./src")
file(GLOB_RECURSE sources main.cpp src/*.cpp src/*.h)


add_executable(dynamical ${sources})


include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(dynamical ${SDL2_LIBRARIES})
