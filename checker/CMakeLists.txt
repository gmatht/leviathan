cmake_minimum_required (VERSION 2.8.11)
project (checker CXX)

# Prevent Boost library autolink
add_definitions ("-DBOOST_ALL_NO_LIB")

#
# Checker executable
#
set (SRC main.cpp)

add_executable (checker ${SRC})

target_include_directories(
  checker 
  PRIVATE 
    checker/ 
    "${CPPUTILS_HEADERS}"
    "${TCLAP_HEADERS}"
    "${STDOPTIONAL_HEADERS}"
)

if (NOT (CMAKE_SYSTEM_NAME MATCHES "Windows"))
	target_compile_definitions(checker PRIVATE HAVE_LONG_LONG)
endif ()

set(
  WARNINGS
  -Wno-global-constructors
  -Wno-exit-time-destructors
)

target_compile_options(checker PUBLIC ${WARNINGS})

set_property(TARGET checker 
             PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries (checker leviathan)

install (TARGETS checker RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
