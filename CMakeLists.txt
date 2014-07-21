cmake_minimum_required (VERSION 3.0)
project (leviathan)

if (NOT CMAKE_BUILD_TYPE)
        message(STATUS "No build type selected, defaults to Release")
        set(CMAKE_BUILD_TYPE "Release")
endif()

set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/parser/lex.cc LANGUAGE CXX)
set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parser.cc LANGUAGE CXX)

set (src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (bin_dir ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

set (src
    ${src_dir}/formula.cpp
    ${src_dir}/generator.cpp
    ${src_dir}/solver.cpp
    ${src_dir}/visitor.cpp
    ${src_dir}/parser/lex.cc
    ${src_dir}/parser/parse.cc
    ${src_dir}/parser/parser.cpp
)

set_source_files_properties (${src} PROPERTIES COMPILE_FLAGS "-std=c++11 -stdlib=libc++")
set_source_files_properties (${src_dir}/main.cpp PROPERTIES COMPILE_FLAGS "-std=c++11 -stdlib=libc++")

include_directories (include)
include_directories (${src_dir})
include_directories (${src_dir}/parser)
include_directories (${src_dir}/tclap)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (CMAKE_CXX_COMPILER /usr/bin/clang++)
    set (CMAKE_C_COMPILER /usr/bin/clang)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set (CMAKE_CXX_FLAGS "-Wall -Wpedantic -Wno-c++98-compat -Wstrict-aliasing -Wunreachable-code -fPIC -march=native -mtune=native")
set (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -fstrict-aliasing -flto -ffast-math -Ofast")

file (MAKE_DIRECTORY ${bin_dir})

set (EXECUTABLE_OUTPUT_PATH ${bin_dir})
set (LIBRARY_OUTPUT_PATH ${bin_dir})

add_library (leviathan SHARED ${src})

link_directories (${bin_dir})

add_executable (checker ${src_dir}/main.cpp)
target_link_libraries (checker leviathan)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (checker -nodefaultlibs c++abi c c++ gcc gcc_s m)
    target_link_libraries (leviathan -nodefaultlibs c++abi c c++ gcc gcc_s m)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
