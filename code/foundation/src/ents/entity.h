#pragma once
#include "platform/system.h"
#include "gen/assets.h"

#define ENTITY_ACTION_VELOCITY_THRESHOLD 0.05f

typedef struct {
    asset_id id;
    uint64_t (*proc)();
} spawndef;

void entity_spawndef_setup();
void entity_spawndef_cleanup();
void entity_spawndef_register(spawndef def);

uint64_t entity_spawn(uint16_t class_id /* 0 = no streaming */);
uint64_t entity_spawn_id(uint16_t id);
void entity_batch_despawn(uint64_t *ids, size_t num_ids);
void entity_despawn(uint64_t ent_id);
void entity_set_position(uint64_t ent_id, float x, float y);


// NOTE(zaklaus): action-based entity stream throttling
void entity_wake(uint64_t ent_id);
void entity_update_action_timers();
bool entity_can_stream(uint64_t ent_id);

