cmake_minimum_required(VERSION 3.0)
project(eco2d)
include(cmake/utils.cmake)

set(CMAKE_C_STANDARD 11)
setup_build_dirs()

include_directories(code/common code/vendors code/vendors/flecs)

add_subdirectory(code/common)
add_subdirectory(code/vendors)

add_subdirectory(code/apps/client)
add_subdirectory(code/apps/server)
