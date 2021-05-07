#pragma once
#include "system.h"
#include "world_view.h"

void game_init(int8_t play_mode, uint32_t num_viewers, int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size);
void game_shutdown();
uint8_t game_is_running();
int8_t game_is_networked();

//~ NOTE(zaklaus): game events
void game_input();
void game_update();
void game_render();

//~ NOTE(zaklaus): world view management
world_view *game_world_view_get_active(void);
world_view *game_world_view_get(uint16_t idx);
void game_world_view_set_active_by_idx(uint16_t idx);
void game_world_view_set_active(world_view *view);
void game_world_view_cycle_active(uint8_t dir);

//~ NOTE(zaklaus): viewer -> host actions
void game_action_send_keystate(double x, double y, uint8_t use, uint8_t sprint);