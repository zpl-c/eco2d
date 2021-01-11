add_executable(eco2d-server
  source/main.c
  source/network.c
  source/perlin.c
  source/options.c
  source/world.c
  source/blocks.c

  header/network.h
  header/perlin.h
  header/options.h
  header/world.h
  header/world_gen.h
  header/blocks.h
  header/blocks_info.h
)

include_directories(eco2d-server header)
target_link_libraries(eco2d-server raylib)
