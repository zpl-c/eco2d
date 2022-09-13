cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0057 NEW)
project(eco2d)
include(cmake/utils.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
SET(CMAKE_USE_RELATIVE_PATHS OFF)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (EMSCRIPTEN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 --profiling -s ASSERTIONS=1 -s WASM=1 -s INITIAL_MEMORY=268435456 -s FORCE_FILESYSTEM=1 --preload-file ${CMAKE_SOURCE_DIR}/art@art/ --shell-file ${CMAKE_SOURCE_DIR}/web/eco2d.html")
  set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
  set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
endif ()

include_directories(code/common code/vendors code/vendors/flecs)

include(cmake/FindRaylib.cmake)

add_subdirectory(code/modules)
add_subdirectory(code/vendors)

add_subdirectory(code/game)
