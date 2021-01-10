cmake_minimum_required(VERSION 3.15)
project(eco2d)

set(CMAKE_C_STANDARD 11)

include_directories(eco2d-client code/vendors)
include_directories(eco2d-server code/vendors)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

add_subdirectory(code/apps/client)
add_subdirectory(code/apps/server)
