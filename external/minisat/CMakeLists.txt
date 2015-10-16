cmake_minimum_required (VERSION 3.1)
project (minisat CXX)

#
# Compilation of minisat dependency
#
add_library(minisat STATIC minisat/core/Solver.cc)
target_include_directories(minisat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET minisat PROPERTY CXX_STANDARD 11)
set_property(TARGET minisat PROPERTY CXX_STANDARD_REQUIRED ON)