cmake_minimum_required (VERSION 3.1)
project (leviathan)

enable_language(C)
enable_language(CXX)

find_package(Boost 1.36.0 REQUIRED COMPONENTS system)

set (src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (bin_dir ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set (src
    ${src_dir}/ast/formula.cpp
    ${src_dir}/ast/generator.cpp
    ${src_dir}/ast/simplifier.cpp
    ${src_dir}/ast/pretty_printer.cpp
    ${src_dir}/ast/clause_counter.cpp
    ${src_dir}/solver.cpp
    ${src_dir}/parser/lex.cc
    ${src_dir}/parser/parse.cc
    ${src_dir}/parser/parser.cpp
    ${src_dir}/minisat/core/Solver.cc
)

set (checker_src
    ${src_dir}/main.cpp
    ${src_dir}/json_output.cpp
)

include_directories (include)
include_directories (${src_dir})
include_directories (${src_dir}/ast)
include_directories (${src_dir}/parser)


set(CMAKE_CXX_FLAGS "-std=c++1y -DHAVE_LONG_LONG")

add_library (leviathan ${src})
target_link_libraries (leviathan ${Boost_LIBRARIES})

link_directories (${bin_dir})

add_executable (checker ${checker_src})
target_link_libraries (checker leviathan)
