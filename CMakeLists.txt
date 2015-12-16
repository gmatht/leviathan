cmake_minimum_required (VERSION 2.8.11)

if (NOT (CMAKE_BUILD_TYPE MATCHES ""))
	message(STATUS "Configuring in ${CMAKE_BUILD_TYPE} mode")
endif ()

project (leviathan)

include (GNUInstallDirs)

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

#
# Testing
#
if (ENABLE_TESTS MATCHES "yes")
  enable_testing()

  execute_process(
    COMMAND find tests -type f -name "*.alaska"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE TESTS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Make the output into a CMake list
  string(REPLACE "\n" ";" TESTS ${TESTS})

  foreach(TEST IN LISTS TESTS)
    execute_process(COMMAND echo ${TEST} OUTPUT_VARIABLE TESTNAME)
    string(STRIP ${TESTNAME} TESTNAME)
    add_test(
      NAME ${TEST}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND checker --parsable ${TEST}
    )
  endforeach()
endif()
