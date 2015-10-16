cmake_minimum_required (VERSION 3.1)
project (lib CXX)

find_package(Boost 1.36.0 REQUIRED COMPONENTS system)

#
# Leviathan library
#
set (
  SRC 
  src/ast/formula.cpp
  src/ast/generator.cpp
  src/ast/simplifier.cpp
  src/ast/pretty_printer.cpp
  src/ast/clause_counter.cpp
  src/solver.cpp
  src/parser/lex.cc
  src/parser/parse.cc
  src/parser/parser.cpp
  ../external/minisat/minisat/core/Solver.cc
)

add_library (leviathan ${SRC})

target_include_directories(
  leviathan 
  PRIVATE ${Boost_INCLUDES} src/ src/ast src/parser include/
)

target_link_libraries (leviathan ${Boost_LIBRARIES})



target_compile_features(leviathan PRIVATE cxx_constexpr)

set_property(TARGET leviathan PROPERTY CXX_STANDARD 11)
set_property(TARGET leviathan PROPERTY CXX_STANDARD_REQUIRED ON)

