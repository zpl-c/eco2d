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

if (WIN32)
  target_link_libraries(eco2d-server winmm)
elseif (APPLE)
  target_link_libraries(eco2d-server pthread m dl)
elseif (UNIX)
  target_link_libraries(eco2d-server pthread m dl atomic)
endif()
