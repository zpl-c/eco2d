include(FindRaylib.cmake)

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

set(LIBS client-common cwpack eco2d-common eco2d-modules flecs-bundle)

include_directories(header ../../modules)
target_link_libraries(eco2d-client raylib ${LIBS})
target_link_libraries(eco2d-cli ${LIBS})

link_system_libs(eco2d-cli)
link_system_libs(eco2d-client)
