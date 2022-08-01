#pragma once
#include "system.h"

#define ENTITY_ACTION_VELOCITY_THRESHOLD 0.05f

uint64_t entity_spawn(uint16_t class_id /* 0 = no streaming */);
void entity_batch_despawn(uint64_t *ids, size_t num_ids);
void entity_despawn(uint64_t ent_id);
void entity_set_position(uint64_t ent_id, float x, float y);

// NOTE(zaklaus): action-based entity stream throttling
void entity_wake(uint64_t ent_id);
void entity_update_action_timers();
bool entity_can_stream(uint64_t ent_id);
