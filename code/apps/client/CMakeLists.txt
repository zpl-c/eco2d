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

add_library(client-common STATIC
  source/network.c
  source/game.c
  source/main.c

  header/network.h
  header/game.h
)

add_executable(eco2d-client
  source/platform_raylib.c
)

add_executable(eco2d-cli
  source/platform_text.c
)

set(LIBS client-common cwpack eco2d-common)

include_directories(header/)
target_link_libraries(eco2d-client raylib ${LIBS})
target_link_libraries(eco2d-cli ${LIBS})

link_system_libs(eco2d-cli)
link_system_libs(eco2d-client)
