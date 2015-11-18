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
  src/format.cpp
)

set (
  HEADERS
  include/formula.hpp
  include/frame.hpp
  include/identifiable.hpp
  include/leviathan.hpp
  include/model.hpp
  include/parser.hpp
  include/pretty_printer.hpp
  include/simplifier.hpp
  include/solver.hpp
  include/visitor.hpp
  include/format.hpp
)

# Here adding the headers is redundant, but makes them appear in 
# project files made by IDE generators (Xcode, VS, ecc...)
add_library (leviathan ${SRC} ${HEADERS})

target_include_directories(
  leviathan 
  PRIVATE src/
  PUBLIC 
    include/ 
    "${CPPUTILS_HEADERS}"
    "${TCLAP_HEADERS}"
    "${STDOPTIONAL_HEADERS}"
)

# Dependencies
target_link_libraries (leviathan minisat cppformat ${Boost_LIBRARIES})
