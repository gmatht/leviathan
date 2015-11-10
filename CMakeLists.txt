cmake_minimum_required (VERSION 2.8.11)

message(STATUS "Configuring in ${CMAKE_BUILD_TYPE} mode")

project (leviathan)


include(GNUInstallDirs)

# Globally set C++11 standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(external/cppformat)
add_subdirectory(external/minisat)
add_subdirectory(lib)
add_subdirectory(checker)
