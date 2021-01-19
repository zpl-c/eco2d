add_executable(eco2d-server
  source/main.c
  source/network.c
  source/utils/options.c
  source/world/perlin.c
  source/world/world.c
  source/world/blocks.c

  header/network.h
  header/platform.h
  header/utils/options.h
  header/world/perlin.h
  header/world/world.h
  header/world/blocks.h
  header/world/blocks_info.h

  header/components/net.h
  header/components/physics.h
  header/components/general.h
  header/components/controllers.h
)

include_directories(eco2d-server header)
target_link_libraries(eco2d-server eco2d-common flecs-bundle cwpack)
link_system_libs(eco2d-server)
