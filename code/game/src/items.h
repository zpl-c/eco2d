#pragma once
#include "system.h"
#include "assets.h"
#include "world/blocks.h"

#include "modules/components.h"

#define ITEMS_INVALID 0xFF

typedef enum {
    IKIND_DEMO_ICEMAKER,
} item_kind;

typedef enum {
    UKIND_PLACE,
} item_usage;

typedef struct {
    item_kind kind;
    item_usage usage;
    asset_id asset;
    uint32_t max_quantity;
    
    // NOTE(zaklaus): usage data
    union {
        struct {
            block_biome biome;
            block_kind kind;
        } place;
    };
} item_desc;

// NOTE(zaklaus): item drops
uint64_t item_spawn(item_kind kind, uint32_t qty);
void item_despawn(uint64_t id);

// NOTE(zaklaus): items
uint16_t item_find(item_kind kind);
void item_use(ecs_world_t *ecs, ItemDrop *it, Position p);

uint32_t item_max_quantity(uint16_t id);
item_usage item_get_usage(uint16_t id);

// NOTE(zaklaus): client

asset_id item_get_asset(uint16_t id);