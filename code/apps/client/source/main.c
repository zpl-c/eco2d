#define ZPL_IMPL
#include "zpl.h"
#include "system.h"
#include "game.h"
#include "world/world.h"
#include "packets/packet.h"
#include "signal_handling.h"
#include "network.h"

#include "flecs/flecs.h"
#include "flecs/flecs_dash.h"
#include "flecs/flecs_systems_civetweb.h"
#include "flecs/flecs_os_api_stdcpp.h"

#define DEFAULT_WORLD_SEED 302097
#define DEFAULT_BLOCK_SIZE 64 /* amount of units within a block (single axis) */
#define DEFAULT_CHUNK_SIZE 3 /* amount of blocks within a chunk (single axis) */
#define DEFAULT_WORLD_SIZE 8 /* amount of chunks within a world (single axis) */

static WORLD_PKT_READER(pkt_reader) {
    pkt_header header = {0};
    uint32_t ok = pkt_header_decode(&header, data, datalen);
    
    if (ok && header.ok) {
        return pkt_handlers[header.id].handler(&header);
    } else {
        zpl_printf("[warn] unknown packet id %d (header %d data %d)\n", header.id, ok, header.ok);
    }
    return -1;
}

static WORLD_PKT_WRITER(sp_pkt_writer) {
    return world_read(pkt->data, pkt->datalen);
}

static WORLD_PKT_WRITER(mp_pkt_writer) {
    return 0; // TODO(zaklaus): enet send packet
}

inline int32_t pkt_world_write(pkt_messages id, size_t pkt_size, int8_t is_reliable) {
    pkt_header pkt;
    PKT_IF(pkt_prep_msg(&pkt, id, pkt_size, is_reliable));
    return world_write(&pkt);
}

int main(int argc, char** argv)
{
    zpl_opts opts={0};
    zpl_opts_init(&opts, zpl_heap(), argv[0]);
    
    zpl_opts_add(&opts, "?", "help", "the HELP section", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "s", "single-player", "play single-player game.", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "s", "seed", "world seed", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "r", "random-seed", "generate random world seed", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "bs", "block-size", "amount of units within a block (single axis)", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "cs", "chunk-size", "amount of blocks within a chunk (single axis)", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "ws", "world-size", "amount of chunks within a world (single axis)", ZPL_OPTS_INT);
    
    uint32_t ok = zpl_opts_compile(&opts, argc, argv);
    
    if (!ok) {
        zpl_opts_print_errors(&opts);
        zpl_opts_print_help(&opts);
        return -1;
    }
    
    uint8_t is_networked_play = !zpl_opts_has_arg(&opts, "single-player");
    int32_t seed = zpl_opts_integer(&opts, "seed", DEFAULT_WORLD_SEED);
    uint16_t block_size = zpl_opts_integer(&opts, "block-size", DEFAULT_BLOCK_SIZE);
    uint16_t chunk_size = zpl_opts_integer(&opts, "chunk-size", DEFAULT_CHUNK_SIZE);
    uint16_t world_size = zpl_opts_integer(&opts, "world-size", DEFAULT_WORLD_SIZE);
    
    if (zpl_opts_has_arg(&opts, "random-seed")) {
        zpl_random rnd={0};
        zpl_random_init(&rnd);
        seed = zpl_random_gen_u32(&rnd);
        zpl_printf("Seed: %u\n", seed);
    }
    
    sighandler_register();
    stdcpp_set_os_api();
    game_init();
    
    if (is_networked_play) {
        world_init_minimal(0, 0, 0, pkt_reader, mp_pkt_writer);
        network_init();
        network_client_connect("127.0.0.1", 27000);
    } else {
        world_init(seed, block_size, chunk_size, world_size, pkt_reader, sp_pkt_writer);
        
        pkt_01_welcome table = {.block_size = block_size, .chunk_size = chunk_size, .world_size = world_size};
        pkt_world_write(MSG_ID_01_WELCOME, pkt_01_welcome_encode(&table), 1);
    }
    
    /* server dashboard */
    {
        ECS_IMPORT(world_ecs(), FlecsDash);
        ECS_IMPORT(world_ecs(), FlecsSystemsCivetweb);
        
        ecs_set(world_ecs(), 0, EcsDashServer, {.port = 27001});
        ecs_set_target_fps(world_ecs(), 60);
    }
    
    while (game_is_running())
    {
        game_input();
        game_update();
        game_render();
        if (is_networked_play) network_client_tick();
        else world_update();
    }

    game_shutdown();
    sighandler_unregister();
    
    if (is_networked_play) {
        network_client_disconnect();
        network_destroy();
    } else {
        world_destroy();
    }
    
    zpl_opts_free(&opts);
    return 0;
}
