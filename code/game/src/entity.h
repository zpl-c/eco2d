#pragma once
#include "system.h"

uint64_t entity_spawn(uint16_t class_id /* 0 = no streaming */);
void entity_batch_despawn(uint64_t *ids, size_t num_ids);
void entity_despawn(uint64_t ent_id);
