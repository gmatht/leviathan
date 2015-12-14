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
  minisat/mtl/Alg.h
  minisat/mtl/Alloc.h
  minisat/mtl/Heap.h
  minisat/mtl/IntMap.h
  minisat/mtl/IntTypes.h
  minisat/mtl/Map.h
  minisat/mtl/Sort.h
  minisat/mtl/Vec.h
  minisat/mtl/XAlloc.h
)

# Here adding the headers is redundant, but makes them appear in 
# project files made by IDE generators (Xcode, VS, ecc...)
add_library(minisat STATIC ${SRC} ${HEADERS})

target_compile_options(
  minisat PUBLIC
  "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-std=c++11>"
)

target_include_directories(minisat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
