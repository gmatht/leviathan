cmake_minimum_required (VERSION 2.8.11)
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
)

add_library (leviathan ${SRC})

target_include_directories(
  leviathan 
  PRIVATE src/
  PUBLIC include/
)

# Required C++11 language features
set_property(TARGET leviathan PROPERTY CXX_STANDARD 11)
set_property(TARGET leviathan PROPERTY CXX_STANDARD_REQUIRED ON)

# Dependencies
target_link_libraries (leviathan PUBLIC minisat ${Boost_LIBRARIES})
