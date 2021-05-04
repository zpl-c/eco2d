add_executable(eco2d-server
  source/main.c
  source/network.c
  source/game.c
  source/utils/options.c

  header/network.h
  header/utils/options.h
)

include_directories(header ../../modules)
target_compile_definitions(eco2d-server PRIVATE SERVER)
target_link_libraries(eco2d-server eco2d-common eco2d-modules flecs-bundle cwpack)
link_system_libs(eco2d-server)
