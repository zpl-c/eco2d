#include "core/game.h"
#include "zpl.h"
#include "platform/platform.h"
#include "world/world.h"
#include "pkt/packet.h"
#include "platform/signal_handling.h"
#include "net/network.h"
#include "ents/entity.h"
#include "ents/items.h"
#include "world/world_view.h"
#include "world/entity_view.h"
#include "core/camera.h"
#include "platform/profiler.h"

#include "flecs/flecs_os_api_stdcpp.h"
#include "flecs/flecs.h"

#include "ecs/components.h"
#include "ecs/systems.h"

#include "packets/pkt_00_init.h"
#include "packets/pkt_01_welcome.h"
#include "packets/pkt_switch_viewer.h"

static uint8_t game_mode;
static uint8_t game_should_close;

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
    (void)udata;
    return world_read(pkt->data, pkt->datalen, 0);
}

static WORLD_PKT_WRITER(mp_pkt_writer) {
    if (pkt->is_reliable) {
        return network_msg_send(udata, pkt->data, pkt->datalen, pkt->channel_id);
    }
    else {
        return network_msg_send_unreliable(udata, pkt->data, pkt->datalen, pkt->channel_id);
    }
}

static WORLD_PKT_WRITER(mp_cli_pkt_writer) {
    (void)udata;
    if (pkt->is_reliable) {
        return network_msg_send(0, pkt->data, pkt->datalen, pkt->channel_id);
    }
    else {
        return network_msg_send_unreliable(0, pkt->data, pkt->datalen, pkt->channel_id);
    }
}

void world_viewers_init(uint32_t num_viewers) {
    zpl_buffer_init(world_viewers, zpl_heap(), num_viewers);

    for (uint32_t i = 0; i < num_viewers; i++) {
        zpl_buffer_append(world_viewers, world_view_create(i));
    }
}

void world_viewers_destroy() {
    for (zpl_isize i = 0; i < zpl_buffer_count(world_viewers); i++) {
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

void game_world_view_cycle_active(int8_t dir) {
    uint16_t idx = (uint16_t)(active_viewer - world_viewers);
    game_world_view_set_active_by_idx(zpl_max(0, (idx+dir)%zpl_buffer_count(world_viewers)));
}
void game_world_view_set_active_by_idx(uint16_t idx) {
    ZPL_ASSERT(idx >= 0 && idx < zpl_buffer_count(world_viewers));
    game_world_view_set_active(&world_viewers[idx]);
}

void game_world_view_active_entity_map(void (*map_proc)(uint64_t key, entity_view * value)) {
    entity_view_map(&active_viewer->entities, map_proc);
}

entity_view *game_world_view_active_get_entity(uint64_t ent_id) {
    return entity_view_get(&active_viewer->entities, ent_id);
}

void game_world_view_set_active(world_view *view) {
    active_viewer = view;
    camera_set_follow(view->owner_id);
    pkt_switch_viewer_send(view->view_id);
}

size_t game_world_view_count(void) {
    return zpl_buffer_count(world_viewers);
}

void flecs_dash_init() {
#if !defined(ZPL_SYSTEM_EMSCRIPTEN)
    ecs_singleton_set(world_ecs(), EcsRest, {0});
    ECS_IMPORT(world_ecs(), FlecsMonitor);
#endif
}

float game_time() {
    return (float)get_cached_time();
}

void game_init(const char *ip, uint16_t port, game_kind play_mode, uint32_t num_viewers, int32_t seed, uint16_t chunk_size, uint16_t chunk_amount, int8_t is_dash_enabled) {
    game_mode = play_mode;
    game_should_close = false;

#ifndef _DEBUG
    const char *host_ip = "lab.zakto.pw";
#else
    const char *host_ip = "127.0.0.1";
#endif

    uint16_t host_port = (port > 0) ? port : 27000;

    if (ip != NULL) {
        host_ip = ip;
    }

    // NOTE: initialise subsystems
    {
        assets_setup();
        blocks_setup();
        item_setup();
        entity_spawndef_setup();
    }

    if (game_mode != GAMEKIND_HEADLESS) {
        platform_init();

        world_viewers_init(num_viewers);
        active_viewer = &world_viewers[0];
        camera_reset();
    }

    if (game_mode != GAMEKIND_SINGLE) {
        network_init();
    }

    if (game_mode == GAMEKIND_CLIENT) {
        world_setup_pkt_handlers(pkt_reader, mp_cli_pkt_writer);
        network_client_connect(host_ip, host_port);
    } else {
        stdcpp_set_os_api();
        world_setup_pkt_handlers(pkt_reader, game_mode == GAMEKIND_SINGLE ? sp_pkt_writer : mp_pkt_writer);
        world_init(seed, chunk_size, chunk_amount);
        if (is_dash_enabled) flecs_dash_init();

        if (game_mode == GAMEKIND_HEADLESS) {
            network_server_start(0, 27000);
            //ecs_set_target_fps(world_ecs(), 60);
        }
    }

    if (game_mode == GAMEKIND_SINGLE) {
        for (uint32_t i = 0; i < num_viewers; i++) {
            pkt_00_init_send(i);
        }
    }
}

int8_t game_is_networked() {
    return game_mode != GAMEKIND_SINGLE;
}

void game_shutdown() {

    if (game_mode == GAMEKIND_CLIENT) {
        network_client_disconnect();
    } else {
        world_destroy();

        if (game_mode == GAMEKIND_HEADLESS) {
            network_server_stop();
        }
    }

    if (game_mode != GAMEKIND_SINGLE) {
        network_destroy();
    }

    if (game_mode != GAMEKIND_HEADLESS) {
        world_viewers_destroy();

        // TODO(zaklaus): crashes on exit
        //platform_shutdown();
    }

    // NOTE: shutdown subsystems
    {
        item_cleanup();
        entity_spawndef_cleanup();
        blocks_cleanup();
        assets_cleanup();
    }
}

uint8_t game_is_running() {
    uint8_t is_running = !game_should_close;
    if (game_mode != GAMEKIND_HEADLESS) {
        is_running = platform_is_running();
    }
    return is_running;
}

game_kind game_get_kind(void) {
    return game_mode;
}

void game_input() {
    if (game_mode != GAMEKIND_HEADLESS) {
        platform_input();
    }
}

void game_update() {
    static double last_update = 0.0f;
    if (game_mode == GAMEKIND_CLIENT) {
        network_client_tick();
    }
    else {
        world_update();

        if (game_mode == GAMEKIND_HEADLESS) {
            network_server_tick();

            static float ms_report = 2.5f;
            if (ms_report < get_cached_time()) {
                ms_report = get_cached_time() + 5.f;
                zpl_printf("delta: %f ms.\n", (get_cached_time() - last_update)*1000.0f);
            }
        }
    }

    last_update = get_cached_time();
}

void game_render() {
    if (game_mode != GAMEKIND_HEADLESS) {
        platform_render();
    }
}

void game_action_send_keystate(game_keystate_data *data) {
    pkt_send_keystate_send(active_viewer->view_id, data);
}

void game_action_send_blockpos(float mx, float my) {
    pkt_send_blockpos data = {
        .mx = mx,
        .my = my
    };
    pkt_send_blockpos_send(active_viewer->view_id, &data);
}

void game_request_close() {
    game_should_close = true;
    if (game_mode != GAMEKIND_HEADLESS) {
        platform_request_close();
    }
}
