cmake_minimum_required (VERSION 2.8.11)

if (NOT (CMAKE_BUILD_TYPE MATCHES ""))
	message(STATUS "Configuring in ${CMAKE_BUILD_TYPE} mode")
endif ()

project (leviathan)

include (GNUInstallDirs)

if (NOT (CMAKE_SYSTEM_NAME MATCHES "Windows"))
	# Globally set C++11 standard
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
endif ()

message(${CMAKE_GENERATOR})

if(CMAKE_GENERATOR MATCHES "Xcode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot /")
endif ()

add_subdirectory (external/cppformat)
add_subdirectory (external/minisat)
add_subdirectory (external/cpputils)
add_subdirectory (external/tclap)
add_subdirectory (external/optional)
add_subdirectory (lib)
add_subdirectory (checker)
