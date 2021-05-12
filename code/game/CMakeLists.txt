file(GLOB PKT_SRCS ../game/header/packets/*.h ../game/source/packets/*.c)

add_executable(eco2d
	source/platform_raylib.c
	source/main.c

	source/network.c
	source/game.c
	source/camera.c
	source/world_view.c
	source/prediction.c

	source/assets.c
	source/compress.c
	source/entity.c
	source/entity_view.c
	source/packet.c
	source/player.c
	source/signal_handling.c
  
	source/utils/options.c

	header/network.h

	source/world/blocks.c
	source/world/perlin.c
	source/world/world.c

	source/gen/texgen.c

	source/world/worldgen/worldgen_test.c

	${PKT_SRCS}
)

target_compile_definitions(eco2d PRIVATE CLIENT)
include_directories(header ../modules)
target_link_libraries(eco2d raylib cwpack eco2d-modules flecs-bundle)

link_system_libs(eco2d)
