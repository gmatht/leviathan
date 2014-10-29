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

set(FORMULA_SIZE "64" CACHE STRING "The maximum size of the formula")

message (STATUS "Maximum formula size: " ${FORMULA_SIZE})

set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/parser/lex.cc LANGUAGE CXX)
set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parser.cc LANGUAGE CXX)

set (src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (bin_dir ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set (src
    ${src_dir}/formula.cpp
    ${src_dir}/generator.cpp
    ${src_dir}/solver.cpp
    ${src_dir}/visitor.cpp
    ${src_dir}/parser/lex.cc
    ${src_dir}/parser/parse.cc
    ${src_dir}/parser/parser.cpp
)

set (checker_src
    ${src_dir}/main.cpp
    ${src_dir}/jsonOutput.cpp
)

include_directories (include)
include_directories (${src_dir})
include_directories (${src_dir}/parser)
include_directories (${src_dir}/tclap)

if (${CMAKE_CXX_COMPILER} MATCHES "usr/bin/c\\+\\+")
    set (CMAKE_CXX_COMPILER /usr/bin/clang++)
    set (CMAKE_C_COMPILER /usr/bin/clang)
endif ()

if (${CMAKE_CXX_COMPILER} MATCHES "usr/bin/clang\\+\\+")
    set_source_files_properties (${src} PROPERTIES COMPILE_FLAGS "-stdlib=libc++")
    set_source_files_properties (${checker_src} PROPERTIES COMPILE_FLAGS "-stdlib=libc++")
endif()

set (CMAKE_CXX_FLAGS "-Wall -Wpedantic -Wstrict-aliasing -Wunreachable-code -fPIC -march=native -mtune=native -std=c++11")
set (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -fstrict-aliasing -ffast-math -Ofast")

add_definitions (-DMAX_FORMULA_SIZE=${FORMULA_SIZE})

file (MAKE_DIRECTORY ${bin_dir})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin_dir})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${bin_dir})

add_library (leviathan SHARED ${src})

link_directories (${bin_dir})

add_executable (checker ${checker_src})
target_link_libraries (checker leviathan)

if (${CMAKE_CXX_COMPILER} MATCHES "usr/bin/clang\\+\\+" AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (checker -nodefaultlibs c++abi c c++ gcc gcc_s m)
    target_link_libraries (leviathan -nodefaultlibs c++abi c c++ gcc gcc_s m)
endif ()
