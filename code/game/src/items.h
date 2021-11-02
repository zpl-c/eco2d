#pragma once
#include "system.h"
#include "assets.h"
#include "world/blocks.h"

#include "modules/components.h"

typedef enum {
    UKIND_HOLD,
    UKIND_PLACE,
    UKIND_PLACE_ITEM,
    UKIND_END_PLACE,
} item_usage;

typedef struct {
    asset_id kind;
    item_usage usage;
    uint32_t max_quantity;
    
    // NOTE(zaklaus): usage data
    union {
        struct {
            asset_id kind;
        } place;
    };
} item_desc;

// NOTE(zaklaus): item drops
uint64_t item_spawn(asset_id kind, uint32_t qty);
void item_despawn(uint64_t id);

// NOTE(zaklaus): items
uint16_t item_find(asset_id kind);
void item_use(ecs_world_t *ecs, ItemDrop *it, Position p);

uint32_t item_max_quantity(uint16_t id);
item_usage item_get_usage(uint16_t id);
