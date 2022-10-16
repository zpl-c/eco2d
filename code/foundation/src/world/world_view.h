#pragma once
#include "platform/system.h"
#include "world/entity_view.h"
#include "world/world.h"

typedef struct {
    uint16_t view_id;
    uint64_t owner_id;
    entity_view_tbl entities;
    librg_world *tracker;

    uint32_t seed;
    uint32_t size;
    uint32_t dim, chk_dim;
    uint16_t chunk_size;
    uint16_t chunk_amount;

    block_id **block_mapping;
    block_id **outer_block_mapping;
    entity_view **chunk_mapping;

    // NOTE(zaklaus): metrics
    float last_update[WORLD_TRACKER_LAYERS];
    float delta_time[WORLD_TRACKER_LAYERS];
    uint8_t active_layer_id;
} world_view;

world_view world_view_create(uint16_t view_id);
void world_view_init(world_view *view, uint32_t seed, uint64_t ent_id, uint16_t chunk_size, uint16_t chunk_amount);
void world_view_destroy(world_view *view);

void world_view_setup_chunk(world_view *view, entity_view *chk);
void world_view_clear_chunk(world_view *view, entity_view *chk);

typedef struct {
    uint16_t id;
    block_id bid;
    entity_view* chunk_e;
    int64_t chunk_id;
    float ox, oy;
    float aox, aoy;
    bool is_outer;
} world_view_block_lookup;

world_view_block_lookup world_view_block_from_realpos(world_view *view, float x, float y);
