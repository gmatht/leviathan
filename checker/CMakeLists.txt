cmake_minimum_required (VERSION 3.1)
project (checker CXX)

#
# Checker executable
#
set (SRC
  main.cpp
  json_output.cpp
)

add_executable (checker ${SRC})

target_include_directories(
  checker PRIVATE checker/ ../external/tclap
)

target_compile_definitions(checker PRIVATE HAVE_LONG_LONG)

target_link_libraries (checker leviathan)

install(TARGETS checker RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
