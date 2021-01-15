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
  header/blocks.h
  header/blocks_info.h
)

include_directories(eco2d-server header)

if (WIN32)
    target_link_libraries(eco2d-server winmm)
elseif (UNIX)
    target_link_libraries(eco2d-server pthread m dl atomic)
endif()
