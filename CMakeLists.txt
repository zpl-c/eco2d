cmake_minimum_required(VERSION 3.0)
project(eco2d)
include(cmake/utils.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

include_directories(eco2d-cli code/vendors code/common)
include_directories(eco2d-client code/vendors code/common)
include_directories(eco2d-server code/vendors code/vendors/flecs code/common)

add_subdirectory(code/common)
add_subdirectory(code/vendors)

add_subdirectory(code/apps/client)
add_subdirectory(code/apps/server)
