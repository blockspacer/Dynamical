cmake_minimum_required(VERSION 3.0)

project(dynamical)

add_executable(dynamical main.cpp)

install(TARGETS dynamical RUNTIME DESTINATION bin)
