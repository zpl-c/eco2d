#pragma once
#include "platform/system.h"
#include "gen/assets.h"
#include "world/blocks.h"

#include "ecs/components.h"

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
    bool unique;
    
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
            asset_id id;
        } place_item;
    };
} item_desc;

typedef uint16_t item_id;

// NOTE(zaklaus): item drops
void item_show(uint64_t ent, bool show);

uint64_t item_spawn(asset_id kind, uint32_t qty);
void item_despawn(uint64_t id);

// NOTE: item ops
void item_pickup(uint64_t ent, uint64_t item);
void item_drop(uint64_t ent, uint64_t item);
void item_merge(uint64_t item1, uint64_t item2);

// NOTE(zaklaus): items
item_id item_find(asset_id kind);
void item_use(ecs_world_t *ecs, ItemSlot *it, Position p, uint64_t udata);
Item *item_get_data(uint64_t ent);

uint32_t item_max_quantity(item_id id);
item_usage item_get_usage(item_id id);
bool item_get_place_directional(item_id id);
