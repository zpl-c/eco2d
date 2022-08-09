#pragma once
#include "system.h"
#include "world_view.h"
#include "packets/pkt_send_keystate.h"

typedef enum {
    GAMEKIND_SINGLE,
    GAMEKIND_CLIENT,
    GAMEKIND_HEADLESS,
    FORCE_GAMEKIND_UINT8 = UINT8_MAX
} game_kind;

void game_init(const char *ip, uint16_t port, game_kind play_mode, uint32_t num_viewers, int32_t seed, uint16_t chunk_size, uint16_t chunk_amount, int8_t is_dash_enabled);
void game_shutdown();
void game_request_close();
uint8_t game_is_running();
int8_t game_is_networked();
float game_time();
game_kind game_get_kind(void);

//~ NOTE(zaklaus): game events
void game_input();
void game_update();
void game_render();

//~ NOTE(zaklaus): world view management
world_view *game_world_view_get_active(void);
world_view *game_world_view_get(uint16_t idx);
size_t game_world_view_count(void);
void game_world_view_set_active_by_idx(uint16_t idx);
void game_world_view_set_active(world_view *view);
void game_world_view_cycle_active(int8_t dir);
void game_world_view_active_entity_map(void (*map_proc)(uint64_t key, entity_view * value));
entity_view *game_world_view_active_get_entity(uint64_t ent_id);

//~ NOTE(zaklaus): viewer -> host actions
void game_action_send_keystate(game_keystate_data *data);
void game_action_send_blockpos(float mx, float my);
