cmake_minimum_required (VERSION 2.8.11)
project (minisat CXX)

#
# Compilation of minisat dependency
#
set(SRC minisat/core/Solver.cc)
set (
  HEADERS 
  minisat/core/Solver.h
  minisat/core/SolverTypes.h  
)

# Here adding the headers is redundant, but makes them appear in 
# project files made by IDE generators (Xcode, VS, ecc...)
add_library(minisat STATIC ${SRC} ${HEADERS})

target_include_directories(minisat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
