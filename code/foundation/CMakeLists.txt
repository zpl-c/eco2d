file(GLOB PKT_SRCS src/packets/*.h src/packets/*.c)

add_library(eco2d-foundation STATIC
    src/core/game.c
    src/core/camera.c
    src/core/rules_default.c

    src/platform/signal_handling.c
    src/platform/profiler.c

    src/models/assets.c
    src/models/components.c
    src/models/items.c
    src/models/entity.c
    src/models/device.c
    src/models/crafting.c

    src/models/prefabs/player.c
    src/models/prefabs/vehicle.c
    src/models/prefabs/storage.c
    src/models/prefabs/furnace.c
    src/models/prefabs/blueprint.c

    src/pkt/packet.c

    src/gen/texgen_fallback.c

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

    src/systems/systems.c

    ${PKT_SRCS}
)

target_compile_definitions(eco2d-foundation PRIVATE CLIENT)
include_directories(src ../modules ../../art/gen)
target_link_libraries(eco2d-foundation raylib cwpack flecs-bundle vendors-bundle)

link_system_libs(eco2d-foundation)
