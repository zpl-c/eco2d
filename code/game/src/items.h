#pragma once
#include "system.h"
#include "assets.h"
#include "world/blocks.h"

#include "modules/components.h"

typedef enum {
    // NOTE(zaklaus): hardcoded fields for placement ops
    UKIND_DELETE,
    UKIND_PLACE,
    UKIND_PLACE_ITEM,
    UKIND_END_PLACE,
    
    // NOTE(zaklaus): the rest of possible actions
    UKIND_HOLD,
    UKIND_PROXY,
} item_usage;

typedef struct {
    asset_id kind;
    item_usage usage;
    uint32_t max_quantity;
    
    // NOTE(zaklaus): usage data
    union {
        struct {
            asset_id kind;
            bool directional; // NOTE(zaklaus): expects next 4 asset entries to be direction assets
        } place;
        
        struct {
            asset_id id;
        } proxy;
        
        struct {
            uint64_t (*spawn_proc)();
        } place_item;
    };
} item_desc;

typedef uint16_t item_id;

// NOTE(zaklaus): item drops
uint64_t item_spawn(asset_id kind, uint32_t qty);
void item_despawn(uint64_t id);

// NOTE(zaklaus): items
item_id item_find(asset_id kind);
void item_use(ecs_world_t *ecs, ItemDrop *it, Position p, uint64_t udata);

uint32_t item_max_quantity(item_id id);
item_usage item_get_usage(item_id id);
bool item_get_place_directional(item_id id);
