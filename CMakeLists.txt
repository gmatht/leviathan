cmake_minimum_required (VERSION 3.1)
project (leviathan)

enable_language(C)
enable_language(CXX)

add_subdirectory(lib)




#
# Final executable
#
#set (checker_src
#  ${SRC_DIR}/main.cpp
#  ${SRC_DIR}/json_output.cpp
#)

#add_executable (checker ${checker_src})
#target_link_libraries (checker leviathan)