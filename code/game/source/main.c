#define ZPL_IMPL
#include "zpl.h"
#include "system.h"
#include "game.h"
#include "entity.h"
#include "entity_view.h"
#include "utils/options.h"
#include "signal_handling.h"
#include "profiler.h"

#include "flecs/flecs.h"
#include "flecs/flecs_dash.h"
#include "flecs/flecs_systems_civetweb.h"
#include "flecs/flecs_os_api_stdcpp.h"

#include "modules/components.h"
#include "modules/systems.h"

#define DEFAULT_WORLD_SEED 302097
#define DEFAULT_CHUNK_SIZE 16 /* amount of blocks within a chunk (single axis) */
#define DEFAULT_WORLD_SIZE 32 /* amount of chunks within a world (single axis) */

int main(int argc, char** argv) {
    zpl_opts opts={0};
    zpl_opts_init(&opts, zpl_heap(), argv[0]);
    
    zpl_opts_add(&opts, "?", "help", "the HELP section", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "v", "viewer-only", "run viewer-only client", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "c", "viewer-count", "number of viewers (detachable clients)", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "p", "preview-map", "draw world preview", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "ed", "enable-dash", "enables flecs dash", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "s", "seed", "world seed", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "r", "random-seed", "generate random world seed", ZPL_OPTS_FLAG);
    //zpl_opts_add(&opts, "cs", "chunk-size", "amount of blocks within a chunk (single axis)", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "ws", "world-size", "amount of chunks within a world (single axis)", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "n", "npc-count", "amount of demo npcs to spawn", ZPL_OPTS_INT);
    
    uint32_t ok = zpl_opts_compile(&opts, argc, argv);
    
    if (!ok) {
        zpl_opts_print_errors(&opts);
        zpl_opts_print_help(&opts);
        return -1;
    }
    
    int8_t is_viewer_only = zpl_opts_has_arg(&opts, "viewer-only");
    int8_t is_dash_enabled = zpl_opts_has_arg(&opts, "enable-dash");
    int32_t seed = zpl_opts_integer(&opts, "seed", DEFAULT_WORLD_SEED);
    uint16_t num_viewers = zpl_opts_integer(&opts, "viewer-count", 1);
    uint16_t chunk_size = DEFAULT_CHUNK_SIZE; //zpl_opts_integer(&opts, "chunk-size", DEFAULT_CHUNK_SIZE);
    uint16_t world_size = zpl_opts_integer(&opts, "world-size", DEFAULT_WORLD_SIZE);
    uint32_t npc_count = zpl_opts_integer(&opts, "npc-count", 10000);
    
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
    game_init(is_viewer_only, num_viewers, seed, chunk_size, world_size, is_dash_enabled);
    
    // TODO(zaklaus): VERY TEMPORARY -- SPAWN SOME NPCS THAT RANDOMLY MOVE
    {
        ECS_IMPORT(world_ecs(), Components);
        for (uint32_t i = 0; i < npc_count; i++) {
            uint64_t e = entity_spawn(NULL, EKIND_DEMO_NPC);
            ecs_add(world_ecs(), e, EcsDemoNPC);
            Position *pos = ecs_get_mut(world_ecs(), e, Position, NULL);
            pos->x=rand() % world_dim();
            pos->y=rand() % world_dim();        
            
            Velocity *v = ecs_get_mut(world_ecs(), e, Velocity, NULL);
            v->x = (rand()%3-1) * 100;
            v->y = (rand()%3-1) * 100;
        }
    }
    
    while (game_is_running()) {
        profile (PROF_MAIN_LOOP) {
            game_input();
            game_update();
            game_render();
        }
        
        profiler_collate();
    }
    
    game_shutdown();
    sighandler_unregister();
    
    zpl_opts_free(&opts);
    return 0;
}
