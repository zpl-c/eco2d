#pragma once
#include "system.h"

void game_init(int8_t play_mode, int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size);
void game_shutdown();
uint8_t game_is_running();

void game_input();
void game_update();
void game_render();
