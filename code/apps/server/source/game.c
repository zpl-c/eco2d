#include "game.h"
#include "platform.h"
#include "world/world.h"
#include "packets/packet.h"
#include "signal_handling.h"
#include "network.h"

#include "flecs/flecs.h"
#include "flecs/flecs_dash.h"
#include "flecs/flecs_systems_civetweb.h"
#include "flecs/flecs_os_api_stdcpp.h"

static WORLD_PKT_READER(mp_pkt_reader) {
    pkt_header header = {0};
    uint32_t ok = pkt_header_decode(&header, data, datalen);
    
    if (ok && header.ok) {
        return pkt_handlers[header.id].handler(&header) >= 0;
    } else {
        zpl_printf("[warn] unknown packet id %d (header %d data %d)\n", header.id, ok, header.ok);
    }
    return -1;
}

void game_init(int8_t play_mode, int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size) {
    sighandler_register();
    stdcpp_set_os_api();
    
    zpl_printf("[INFO] Generating world of size: %d x %d\n", world_size, world_size);
    world_init(seed, block_size, chunk_size, world_size, mp_pkt_reader, mp_pkt_writer);
    
    /* server dashboard */
    {
        ECS_IMPORT(world_ecs(), FlecsDash);
        ECS_IMPORT(world_ecs(), FlecsSystemsCivetweb);
        
        ecs_set(world_ecs(), 0, EcsDashServer, {.port = 27001});
        ecs_set_target_fps(world_ecs(), 60);
    }
    
    zpl_printf("[INFO] Initializing network...\n");
    network_init();
    network_server_start("0.0.0.0", 27000);
}

int8_t game_is_networked() {
    return 1;
}

void game_shutdown() {
    network_server_stop();
    network_destroy();
    world_destroy();
    sighandler_unregister();
    zpl_printf("Bye!\n");
}

uint8_t game_is_running() {
    return 1;
}

void game_input() {
    
}

void game_update() {
    network_server_tick();
    world_update();
}

void game_render() {
}

