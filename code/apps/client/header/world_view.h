#pragma once
#include "system.h"
#include "entity_view.h"
#include "world/world.h"

typedef struct {
    uint64_t owner_id;
    entity_view_tbl entities;
    librg_world *tracker;
} world_view;

world_view world_view_create(void);
void world_view_init(world_view *view, uint64_t ent_id, uint16_t block_size, uint16_t chunk_size, uint16_t world_size);
void world_view_destroy(world_view *view);
