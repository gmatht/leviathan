cmake_minimum_required (VERSION 2.8.11)
project (checker CXX)

#
# Checker executable
#
set (SRC main.cpp)
set (HEADERS input.hpp)

add_executable (checker ${SRC} ${HEADERS})

target_include_directories(
  checker 
  PRIVATE 
    checker/ 
    "${CPPUTILS_HEADERS}"
    "${TCLAP_HEADERS}"
    "${STDOPTIONAL_HEADERS}"
)

target_compile_definitions(checker PRIVATE HAVE_LONG_LONG)
set_property(TARGET checker 
             PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries (checker leviathan)

install(TARGETS checker RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
