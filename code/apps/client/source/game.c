#include "game.h"
#include "zpl.h"
#include "platform.h"
#include "world/world.h"
#include "packet.h"
#include "signal_handling.h"
#include "network.h"
#include "entity.h"
#include "world_view.h"
#include "entity_view.h"
#include "camera.h"

#include "flecs/flecs.h"
#include "flecs/flecs_dash.h"
#include "flecs/flecs_systems_civetweb.h"
#include "flecs/flecs_os_api_stdcpp.h"

#include "modules/general.h"
#include "modules/physics.h"
#include "modules/controllers.h"

#include "packets/pkt_00_init.h"
#include "packets/pkt_01_welcome.h"
#include "packets/pkt_send_keystate.h"

static int8_t is_viewer_only;

static world_view *world_viewers;
static world_view *active_viewer;

static WORLD_PKT_READER(pkt_reader) {
    pkt_header header = {0};
    uint32_t ok = pkt_header_decode(&header, data, datalen);
    header.udata = udata;

    if (ok && header.ok) {
        return pkt_handlers[header.id].handler(&header) >= 0;
    } else {
        zpl_printf("[warn] unknown packet id %d (header %d data %d)\n", header.id, ok, header.ok);
    }
    return -1;
}

static WORLD_PKT_WRITER(sp_pkt_writer) {
    return world_read(pkt->data, pkt->datalen, (void*)game_world_view_get_active()->owner_id);
}

static WORLD_PKT_WRITER(mp_pkt_writer) {
    if (pkt->is_reliable) {
        return network_msg_send(pkt->data, pkt->datalen);
    }
    else {
        return network_msg_send_unreliable(pkt->data, pkt->datalen);
    }
}

void world_viewers_init(uint32_t num_viewers) {
    zpl_buffer_init(world_viewers, zpl_heap(), num_viewers);
    
    for (uint32_t i = 0; i < num_viewers; i++) {
        zpl_buffer_append(world_viewers, world_view_create(i));
    }
}

void world_viewers_destroy() {
    for (uint32_t i = 0; i < zpl_buffer_count(world_viewers); i++) {
        world_view_destroy(&world_viewers[i]);
    }
    zpl_buffer_free(world_viewers);
}

world_view *game_world_view_get(uint16_t idx) {
    return &world_viewers[idx];
}

world_view *game_world_view_get_active(void) {
    return active_viewer;
}

void game_world_view_cycle_active(uint8_t dir) {
    uint16_t idx = (uint16_t)(active_viewer - world_viewers);
    game_world_view_set_active_by_idx((idx+dir)%zpl_buffer_count(world_viewers));
}
void game_world_view_set_active_by_idx(uint16_t idx) {
    ZPL_ASSERT(idx >= 0 && idx < zpl_buffer_count(world_viewers));
    game_world_view_set_active(&world_viewers[idx]);
}

void game_world_view_active_entity_map(void (*map_proc)(uint64_t key, entity_view * value)) {
    entity_view_map(&active_viewer->entities, map_proc);
}

void game_world_view_set_active(world_view *view) {
    active_viewer = view;
    camera_set_follow(view->owner_id);
}

void flecs_dash_init() {
    ECS_IMPORT(world_ecs(), FlecsDash);
    ECS_IMPORT(world_ecs(), FlecsSystemsCivetweb);
    
    ecs_set(world_ecs(), 0, EcsDashServer, {.port = 27001});
    ecs_set_target_fps(world_ecs(), 60);
}

void game_init(int8_t play_mode, uint32_t num_viewers, int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t chunk_amount, int8_t is_dash_enabled) {
    is_viewer_only = play_mode;
    platform_init();
    world_viewers_init(num_viewers);
    active_viewer = &world_viewers[0];
    camera_reset();

    if (is_viewer_only) {
        world_setup_pkt_handlers(pkt_reader, mp_pkt_writer);
        network_init();
        network_client_connect("127.0.0.1", 27000);
    } else {
        stdcpp_set_os_api();
        world_setup_pkt_handlers(pkt_reader, sp_pkt_writer);
        world_init(seed, block_size, chunk_size, chunk_amount);
        if (is_dash_enabled) flecs_dash_init();
    }
    
    for (uint32_t i = 0; i < num_viewers; i++) {
        pkt_00_init_send(i);
    }
}

int8_t game_is_networked() {
    return is_viewer_only;
}

void game_shutdown() {
    world_viewers_destroy();

    if (is_viewer_only) {
        network_client_disconnect();
        network_destroy();
    } else {
        world_destroy();
    }
}

uint8_t game_is_running() {
    return platform_is_running();
}

void game_input() {
    platform_input();
}

void game_update() {
    if (is_viewer_only) {
        network_client_tick();
    }
    else world_update();
    
    game_world_cleanup_entities();
}

void game_render() {
    platform_render();
}

void game_action_send_keystate(float x, float y, uint8_t use, uint8_t sprint) {
    pkt_send_keystate_send(active_viewer->view_id, x, y, use, sprint);
}

void game_world_cleanup_entities(void) {
    for (int i = 0; i < zpl_buffer_count(world_viewers); i += 1){
        entity_view_tbl *view = &world_viewers[i].entities;
        
        for (int j = 0; j < zpl_array_count(view->entries); j += 1){
            entity_view *e = &view->entries[j];
            if (e->tran_effect == ETRAN_REMOVE) {
                entity_view_tbl_remove(view, e->ent_id);
                j--;
            } 
        }
        
    }
    
}