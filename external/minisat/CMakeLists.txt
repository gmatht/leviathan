cmake_minimum_required (VERSION 2.8.11)
project (minisat CXX)

#
# Compilation of minisat dependency
#
add_library(minisat STATIC minisat/core/Solver.cc)
target_include_directories(minisat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
