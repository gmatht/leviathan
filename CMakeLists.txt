cmake_minimum_required (VERSION 2.8.11)

message(STATUS "Configuring in ${CMAKE_BUILD_TYPE} mode")

project (leviathan)

# These definitions should go to the lib's CMakeLists.txt
# but the Visual Studio generator is a bitch
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_DEBUG_RUNTIME OFF)
find_package(Boost 1.36.0 REQUIRED COMPONENTS system thread)

include(GNUInstallDirs)

if (NOT (CMAKE_SYSTEM_NAME MATCHES "Windows"))
	# Globally set C++11 standard
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

add_subdirectory(external/cppformat)
add_subdirectory(external/minisat)
add_subdirectory(external/cpputils)
add_subdirectory(external/tclap)
add_subdirectory(external/optional)
add_subdirectory(lib)
add_subdirectory(checker)
