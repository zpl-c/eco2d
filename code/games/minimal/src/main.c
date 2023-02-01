#define ZPL_IMPL
#include "zpl.h"
#include "platform/system.h"
#include "core/game.h"
#include "models/entity.h"
#include "world/entity_view.h"
#include "utils/options.h"
#include "platform/signal_handling.h"
#include "platform/profiler.h"

#include "flecs.h"
#include "flecs/flecs_os_api_stdcpp.h"

#include "models/components.h"
#include "systems/systems.h"

#include "platform/arch.h"

#define DEFAULT_WORLD_SEED 302097
#define DEFAULT_CHUNK_SIZE 16 /* amount of blocks within a chunk (single axis) */
#define DEFAULT_WORLD_SIZE 5 /* amount of chunks within a world (single axis) */

int main(int argc, char** argv) {
    zpl_opts opts={0};
    zpl_opts_init(&opts, zpl_heap(), argv[0]);

    zpl_opts_add(&opts, "?", "help", "the HELP section", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "v", "viewer-only", "run viewer-only client", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "d", "server-only", "run dedicated server", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "p", "preview-map", "draw world preview", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "s", "seed", "world seed", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "r", "random-seed", "generate random world seed", ZPL_OPTS_FLAG);
    //zpl_opts_add(&opts, "cs", "chunk-size", "amount of blocks within a chunk (single axis)", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "ws", "world-size", "amount of chunks within a world (single axis)", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "ip", "host", "host IP address", ZPL_OPTS_STRING);
    zpl_opts_add(&opts, "port", "port", "port number", ZPL_OPTS_INT);

    uint32_t ok = zpl_opts_compile(&opts, argc, argv);

    if (!ok) {
        zpl_opts_print_errors(&opts);
        zpl_opts_print_help(&opts);
        return -1;
    }

    int8_t is_viewer_only = zpl_opts_has_arg(&opts, "viewer-only");
    int8_t is_server_only = zpl_opts_has_arg(&opts, "server-only");
    int32_t seed = (int32_t)zpl_opts_integer(&opts, "seed", DEFAULT_WORLD_SEED);
    uint16_t world_size = (uint16_t)zpl_opts_integer(&opts, "world-size", DEFAULT_WORLD_SIZE);
    uint16_t chunk_size = DEFAULT_CHUNK_SIZE; //zpl_opts_integer(&opts, "chunk-size", DEFAULT_CHUNK_SIZE);
    zpl_string host = zpl_opts_string(&opts, "host", NULL);
    uint16_t port = (uint16_t)zpl_opts_integer(&opts, "port", 0);

    game_kind play_mode = GAMEKIND_SINGLE;

    if (is_viewer_only) play_mode = GAMEKIND_CLIENT;
    if (is_server_only) play_mode = GAMEKIND_HEADLESS;

    if (zpl_opts_has_arg(&opts, "random-seed")) {
        zpl_random rnd={0};
        zpl_random_init(&rnd);
        seed = zpl_random_gen_u32(&rnd);
        zpl_printf("Seed: %u\n", seed);
    }

    if (zpl_opts_has_arg(&opts, "preview-map")) {
        generate_minimap(seed, WORLD_BLOCK_SIZE, chunk_size, world_size);
        return 0;
    }

    sighandler_register();
    game_init(host, port, play_mode, 1, seed, chunk_size, world_size, 0);

    game_run();

    game_shutdown();
    sighandler_unregister();

    zpl_string_free(host);
    zpl_opts_free(&opts);
    return 0;
}

//------------------------------------------------------------------------
void game_player_joined(uint64_t ent) {}
void game_player_departed(uint64_t ent) {}