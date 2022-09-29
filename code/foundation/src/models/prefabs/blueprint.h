#pragma once
#include "platform/system.h"

uint64_t blueprint_spawn(uint8_t w, uint8_t h, const uint16_t *plan);
uint64_t blueprint_spawn_udata(void* udata);
void blueprint_despawn(uint64_t id);


