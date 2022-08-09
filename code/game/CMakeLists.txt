file(GLOB PKT_SRCS ../game/src/packets/*.h ../game/src/packets/*.c)

add_executable(eco2d
    src/platform_raylib.c
    src/main.c

    src/network.c
    src/game.c
    src/camera.c
    src/world_view.c
    src/prediction.c

    src/assets.c
    src/items.c
    src/compress.c
    src/entity.c
    src/entity_view.c
    src/packet.c
    src/player.c
    src/vehicle.c
	src/storage.c
    src/signal_handling.c
    src/profiler.c
    src/debug_ui.c
    src/debug_draw.c

    src/utils/options.c

    src/network.h

    src/world/blocks.c
    src/world/perlin.c
    src/world/world.c

    src/gen/texgen.c

    src/world/worldgen/worldgen_test.c

    ${PKT_SRCS}
)

target_compile_definitions(eco2d PRIVATE CLIENT)
include_directories(src ../modules ../../art/gen)
target_link_libraries(eco2d raylib cwpack eco2d-modules flecs-bundle vendors-bundle)

link_system_libs(eco2d)
