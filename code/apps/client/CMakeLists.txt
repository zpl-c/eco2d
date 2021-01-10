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

        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

add_executable(eco2d-client
  source/main.c
  source/network.c
  source/renderer.c
  source/game.c

  header/renderer.h
  header/network.h
  header/game.h
)

include_directories(header/)
target_link_libraries(eco2d-client raylib)
