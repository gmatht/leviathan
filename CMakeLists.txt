cmake_minimum_required (VERSION 3.0)
project (leviathan CXX)

if (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "Debug")
    message (STATUS "Build type selected: " ${CMAKE_BUILD_TYPE})
endif()

if (NOT CMAKE_BUILD_TYPE)
        message (STATUS "No build type selected, defaults to Release")
        set (CMAKE_BUILD_TYPE "Release")
endif()

if (NOT (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "Debug"))
    message (STATUS "Invalid build type selected, defaults to Release")
    set (CMAKE_BUILD_TYPE "Release")
endif()

set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/parser/lex.cc LANGUAGE CXX)
set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parser.cc LANGUAGE CXX)
set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/minisat/core/Solver.cc LANGUAGE CXX)

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

find_package(Boost COMPONENTS system)

include_directories (include)
include_directories (${src_dir})
include_directories (${src_dir}/ast)
include_directories (${src_dir}/parser)

if (${CMAKE_CXX_COMPILER} MATCHES "usr/bin/c\\+\\+")
    set (CMAKE_CXX_COMPILER /usr/bin/clang++)
    set (CMAKE_C_COMPILER /usr/bin/clang)
endif ()

if (${CMAKE_CXX_COMPILER} MATCHES "usr/bin/clang\\+\\+")
    set_source_files_properties (${src} PROPERTIES COMPILE_FLAGS "-stdlib=libc++")
    set_source_files_properties (${checker_src} PROPERTIES COMPILE_FLAGS "-stdlib=libc++")
endif ()

#-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize
set (CMAKE_CXX_FLAGS "-Wall -Wpedantic -Wstrict-aliasing -Wunreachable-code -Wno-zero-length-array -fPIC -DHAVE_LONG_LONG -march=native -std=c++1y")
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -fstrict-aliasing -O3 -g0")

file (MAKE_DIRECTORY ${bin_dir})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin_dir})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${bin_dir})

add_library (leviathan ${src})
target_link_libraries (leviathan ${Boost_LIBRARIES})

if (CMAKE_BUILD_TYPE MATCHES "Debug" AND CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries (leviathan SegFault)
endif ()

link_directories (${bin_dir})

add_executable (checker ${checker_src})
target_link_libraries (checker leviathan)

if (${CMAKE_CXX_COMPILER} MATCHES "usr/bin/clang\\+\\+" AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (checker c++abi c c++ gcc gcc_s m)
    target_link_libraries (leviathan c++abi c c++ gcc gcc_s m)
endif ()
