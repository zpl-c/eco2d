#pragma once
#include "system.h"
#include "entity_view.h"
#include "world/world.h"

typedef struct {
    uint16_t view_id;
    uint64_t owner_id;
    entity_view_tbl entities;
    librg_world *tracker;
    
    uint32_t size;
    uint32_t dim;
    uint16_t block_size;
    uint16_t chunk_size;
    uint16_t chunk_amount;
    
    // NOTE(zaklaus): metrics
    uint64_t last_update[WORLD_TRACKER_LAYERS];
    uint64_t delta_time[WORLD_TRACKER_LAYERS];
    uint8_t active_layer_id;
} world_view;

world_view world_view_create(uint16_t view_id);
void world_view_init(world_view *view, uint64_t ent_id, uint16_t block_size, uint16_t chunk_size, uint16_t chunk_amount);
void world_view_destroy(world_view *view);
