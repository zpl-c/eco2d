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
  header/platform.h
  header/game.h

  ../../common/signal_handling.c
  ../../common/signal_handling.h
)

add_executable(eco2d-client
  source/platform_raylib.c
)

add_executable(eco2d-cli
  source/platform_text.c
)

include_directories(header/)
target_link_libraries(eco2d-client raylib client-common)
target_link_libraries(eco2d-cli client-common)

if (WIN32)
  target_link_libraries(eco2d-cli winmm)
  target_link_libraries(eco2d-client winmm)
elseif (APPLE)
  target_link_libraries(eco2d-cli pthread m dl)
  target_link_libraries(eco2d-client pthread m dl)
elseif (UNIX)
  target_link_libraries(eco2d-cli pthread m dl atomic)
  target_link_libraries(eco2d-client pthread m dl atomic)
endif()
