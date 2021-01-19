file(GLOB MODULES header/modules/*.h source/modules/*.c)
add_library(eco2d-server-modules STATIC ${MODULES})

add_executable(eco2d-server
  source/main.c
  source/network.c
  source/utils/options.c
  source/world/perlin.c
  source/world/world.c
  source/world/blocks.c

  header/network.h
  header/utils/options.h
  header/world/perlin.h
  header/world/world.h
  header/world/blocks.h
  header/world/blocks_info.h
)

include_directories(eco2d-server header)
target_link_libraries(eco2d-server eco2d-common eco2d-server-modules flecs-bundle cwpack)
link_system_libs(eco2d-server)
