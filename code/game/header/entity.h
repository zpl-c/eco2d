#pragma once
#include "system.h"

uint64_t entity_spawn(char *name, uint16_t class_id /* 0 = no streaming */);
void entity_despawn(uint64_t ent_id);
