#include "game.h"
#include "platform.h"
#include "world/world.h"
#include "packet.h"
#include "signal_handling.h"
#include "network.h"
#include "entity_view.h"
#include "camera.h"

#include "flecs/flecs.h"
#include "flecs/flecs_dash.h"
#include "flecs/flecs_systems_civetweb.h"
#include "flecs/flecs_os_api_stdcpp.h"

static int8_t is_networked_play;
static uint64_t sp_player;

static WORLD_PKT_READER(pkt_reader) {
    pkt_header header = {0};
    uint32_t ok = pkt_header_decode(&header, data, datalen);
    
    if (ok && header.ok) {
        return pkt_handlers[header.id].handler(&header) >= 0;
    } else {
        zpl_printf("[warn] unknown packet id %d (header %d data %d)\n", header.id, ok, header.ok);
    }
    return -1;
}

static WORLD_PKT_WRITER(sp_pkt_writer) {
    return world_read(pkt->data, pkt->datalen, NULL);
}

static WORLD_PKT_WRITER(mp_pkt_writer) {
    if (pkt->is_reliable) {
        return network_msg_send(pkt->data, pkt->datalen);
    }
    else {
        return network_msg_send_unreliable(pkt->data, pkt->datalen);
    }
}

void game_init(int8_t play_mode, int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size) {
    is_networked_play = play_mode;
    platform_init();
    entity_view_init();
    camera_reset();
    
    if (is_networked_play) {
        world_init_minimal(0, 0, 0, pkt_reader, mp_pkt_writer);
        network_init();
        network_client_connect("127.0.0.1", 27000);
    } else {
        stdcpp_set_os_api();
        world_init(seed, block_size, chunk_size, world_size, pkt_reader, sp_pkt_writer);
        
        /* server dashboard */
        {
            ECS_IMPORT(world_ecs(), FlecsDash);
            ECS_IMPORT(world_ecs(), FlecsSystemsCivetweb);
            
            ecs_set(world_ecs(), 0, EcsDashServer, {.port = 27001});
            ecs_set_target_fps(world_ecs(), 60);
        }
        
        sp_player = player_spawn("unnamed");
        
        pkt_01_welcome table = {.ent_id = 0, .block_size = block_size, .chunk_size = chunk_size, .world_size = world_size};
        pkt_world_write(MSG_ID_01_WELCOME, pkt_01_welcome_encode(&table), 1, NULL);
    }
}

int8_t game_is_networked() {
    return is_networked_play;
}

void game_shutdown() {
    entity_view_free();

    if (is_networked_play) {
        network_client_disconnect();
        network_destroy();
    } else {
        player_despawn(sp_player);
        world_destroy();
    }
}

uint8_t game_is_running() {
    return platform_is_running();
}

void game_input() {

}

void game_update() {
    if (is_networked_play) network_client_tick();
    else world_update();
}

void game_render() {
    platform_render();
}

