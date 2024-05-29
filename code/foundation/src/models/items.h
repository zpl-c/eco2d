#pragma once
#include "platform/system.h"
#include "models/assets.h"
#include "world/blocks.h"

#include "models/components.h"

typedef enum {
    // NOTE(zaklaus): hardcoded fields for placement ops
    UKIND_DELETE,
    UKIND_PLACE,
    UKIND_PLACE_ITEM,
    UKIND_PLACE_ITEM_DATA,
    UKIND_END_PLACE,
    
    // NOTE(zaklaus): the rest of possible actions
    UKIND_HOLD,
    UKIND_PROXY,
} item_usage;

typedef enum {
    UDATA_NONE,
    UDATA_ENERGY_SOURCE,
} item_attachment;

typedef struct {
    asset_id kind;
    item_usage usage;
    item_attachment attachment;
    uint32_t max_quantity;
    uint8_t has_storage;
    
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
    
    union {
        struct {
            asset_id producer;
            float energy_level;
        } energy_source;
    };
    
    // NOTE: item data
    union {
        struct {
            uint8_t w;
            uint8_t h;
            const asset_id *plan;
        } blueprint;
    };
} item_desc;

typedef uint16_t item_id;

void item_db(void);

// NOTE(zaklaus): item drops
void item_show(uint64_t ent, bool show);

uint64_t item_spawn(asset_id kind, uint32_t qty);
void item_despawn(uint64_t id);

// NOTE(zaklaus): items
item_id item_find(asset_id kind);
item_id item_find_no_proxy(asset_id kind);
void item_use(ecs_world_t *ecs, ecs_entity_t e, Item *it, Position p, uint64_t udata);
Item *item_get_data(uint64_t ent);
const Item *item_get_data_const(uint64_t ent);

uint32_t item_max_quantity(item_id id);
item_usage item_get_usage(item_id id);
bool item_get_place_directional(item_id id);
item_desc item_get_desc(item_id id);
