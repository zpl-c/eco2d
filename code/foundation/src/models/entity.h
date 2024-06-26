#pragma once
#include "platform/system.h"

#define ENTITY_ACTION_VELOCITY_THRESHOLD 0.001f

uint64_t entity_spawn(uint16_t class_id /* 0 = no streaming */);
uint64_t entity_spawn_id(uint16_t id);
uint64_t entity_spawn_id_with_data(uint16_t id, void* udata);
bool entity_spawn_provided(uint16_t id);
void entity_batch_despawn(uint64_t *ids, size_t num_ids);
void entity_despawn(uint64_t ent_id);
void entity_set_position(uint64_t ent_id, float x, float y);

// NOTE(zaklaus): spawndef manager

void entity_add_spawndef(uint16_t id, uint64_t (*proc)()); 
void entity_add_spawndef_data(uint16_t id, uint64_t (*proc)(void*)); 
void entity_default_spawnlist(void);

// NOTE(zaklaus): action-based entity stream throttling
void entity_wake(uint64_t ent_id);
void entity_update_action_timers();
bool entity_can_stream(uint64_t ent_id);

