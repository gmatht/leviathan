cmake_minimum_required (VERSION 2.8)
project (leviathan)

include(GNUInstallDirs)

add_subdirectory(external/minisat)
add_subdirectory(lib)
add_subdirectory(checker)
