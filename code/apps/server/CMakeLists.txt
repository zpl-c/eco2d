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

  ../../common/signal_handling.c
  ../../common/signal_handling.h

  ../../common/assets.h
  ../../common/assets.c

  ../../vendors/cwpack/cwpack.c
  ../../vendors/cwpack/cwpack.h

  ../../vendors/flecs/flecs.c
  ../../vendors/flecs/flecs.h
  ../../vendors/flecs/flecs_meta.c
  ../../vendors/flecs/flecs_meta.h
  ../../vendors/flecs/flecs_json.h
  ../../vendors/flecs/flecs_json.c
  ../../vendors/flecs/flecs_rest.h
  ../../vendors/flecs/flecs_rest.c
  ../../vendors/flecs/flecs_monitor.h
  ../../vendors/flecs/flecs_monitor.c
  ../../vendors/flecs/flecs_player.h
  ../../vendors/flecs/flecs_player.c
  ../../vendors/flecs/flecs_dash.h
  ../../vendors/flecs/flecs_dash.c
  ../../vendors/flecs/flecs_components_http.h
  ../../vendors/flecs/flecs_components_http.c
  ../../vendors/flecs/flecs_systems_civetweb.h
  ../../vendors/flecs/flecs_systems_civetweb.c
  ../../vendors/flecs/flecs_os_api_posix.h
  ../../vendors/flecs/flecs_os_api_posix.c
)

include_directories(eco2d-server header)

if (WIN32)
  target_link_libraries(eco2d-server winmm)
elseif (APPLE)
  target_link_libraries(eco2d-server pthread m dl)
elseif (UNIX)
  target_link_libraries(eco2d-server pthread m dl atomic)
endif()
