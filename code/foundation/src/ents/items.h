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

typedef enum {
    UDATA_NONE,
    UDATA_FUEL,
    UDATA_INGREDIENT,
} item_attachment;

typedef struct {
    asset_id kind;
    item_usage usage;
    item_attachment attachment;
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
            asset_id id;
        } place_item;
    };

    union {
        struct {
            float burn_time;
        } fuel;

        struct {
            asset_id producer;
            asset_id product;
            asset_id additional_ingredient;
        } ingredient;
    };
} item_desc;

typedef uint16_t item_id;

// NOTE(zaklaus): item drops
void item_show(uint64_t ent, bool show);

uint64_t item_spawn(asset_id kind, uint32_t qty);
void item_despawn(uint64_t id);

// NOTE(zaklaus): items
item_id item_find(asset_id kind);
void item_use(ecs_world_t *ecs, ecs_entity_t e, Item *it, Position p, uint64_t udata);
Item *item_get_data(uint64_t ent);
const Item *item_get_data_const(uint64_t ent);

uint32_t item_max_quantity(item_id id);
item_usage item_get_usage(item_id id);
bool item_get_place_directional(item_id id);
