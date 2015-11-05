cmake_minimum_required (VERSION 2.8.11)
project (checker CXX)

#
# Checker executable
#
set (
  SRC
  main.cpp
  json_output.cpp
)

set(
  HEADERS
  json_output.hpp
) 

# Here adding the headers is redundant, but makes them appear in 
# project files made by IDE generators (Xcode, VS, ecc...)
add_executable (checker ${SRC} ${HEADERS})

target_include_directories(
  checker PRIVATE checker/ ../external/tclap
)

target_compile_definitions(checker PRIVATE HAVE_LONG_LONG)
set_property(TARGET checker 
             PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries (checker leviathan)

install(TARGETS checker RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
