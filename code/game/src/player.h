#pragma once
#include "system.h"

uint64_t player_spawn(char *name);
void player_despawn(uint64_t ent_id);

void player_freeze(uint64_t id, uint8_t state, uint8_t clear);