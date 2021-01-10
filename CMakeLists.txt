cmake_minimum_required(VERSION 3.15)
project(eco2d)

find_package(raylib 3.5 QUIET)

if (NOT raylib_FOUND)
  include(FetchContent)

  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/master.tar.gz
  )

  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    # build raylib
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()


set(CMAKE_C_STANDARD 11)

add_executable(eco2d code/apps/client/main.c)

include_directories(eco2d code/vendors)

target_link_libraries(eco2d raylib)
