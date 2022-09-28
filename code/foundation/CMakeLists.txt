file(GLOB PKT_SRCS src/packets/*.h src/packets/*.c)

add_library(eco2d-foundation STATIC
    src/core/game.c
    src/core/camera.c
    src/core/rules_default.c

    src/platform/signal_handling.c
    src/platform/profiler.c

    src/ents/items.c
    src/ents/entity.c
    src/ents/player.c
    src/ents/vehicle.c
    src/ents/storage.c
    src/ents/device.c
    src/ents/furnace.c

    src/pkt/packet.c

    src/debug/debug_ui.c
    src/debug/debug_draw.c

    src/utils/options.c
    src/utils/compress.c

    src/net/network_enet.c

    src/world/blocks.c
    src/world/perlin.c
    src/world/world.c
    src/world/world_view.c
    src/world/entity_view.c
    src/world/prediction.c

    src/gen/assets.c

    src/ecs/systems.c
	  src/ecs/components.c

    ${PKT_SRCS}
)

target_compile_definitions(eco2d-foundation PRIVATE CLIENT)
include_directories(src ../modules ../../art/gen)
target_link_libraries(eco2d-foundation raylib cwpack flecs-bundle vendors-bundle)
target_compile_options(eco2d-foundation PRIVATE -Werror -Wall -Wextra -Wno-unused-function -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter)

link_system_libs(eco2d-foundation)