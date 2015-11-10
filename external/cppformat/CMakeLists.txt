cmake_minimum_required (VERSION 2.8.11)
project (minisat CXX)

#
# Compilation of minisat dependency
#
set(SRC cppformat/format.cc)
set(HEADERS cppformat/format.h)

# Here adding the headers is redundant, but makes them appear in 
# project files made by IDE generators (Xcode, VS, ecc...)
add_library(cppformat STATIC ${SRC} ${HEADERS})

target_compile_definitions(cppformat PUBLIC FMT_USE_VARIADIC_TEMPLATES)

target_include_directories(cppformat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
