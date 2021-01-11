#pragma once
#include "system.h"

void game_init();
void game_shutdown();
uint8_t game_is_running();

void game_input();
void game_update();
void game_render();
