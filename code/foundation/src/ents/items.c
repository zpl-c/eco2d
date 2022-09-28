#include "ents/items.h"
#include "ents/entity.h"
#include "world/entity_view.h"
#include "world/world.h"
#include "world/blocks.h"

#include "ecs/components.h"

#include "zpl.h"

#include "items_list.c"
#define ITEMS_COUNT (sizeof(items)/sizeof(item_desc))

static inline item_id item_resolve_proxy(item_id id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    item_usage usage = items[id].usage;
    if (usage == UKIND_PROXY) {
        return item_find(items[id].proxy.id);
    }
    return id;
}

static inline asset_id item_fix_kind(asset_id id) {
    return items[item_find(id)].kind;
}

void item_show(uint64_t ent, bool show) {
    Classify *c = ecs_get_mut(world_ecs(), ent, Classify);
    c->id = show ? EKIND_ITEM : EKIND_SERVER;
}

uint64_t item_spawn(asset_id kind, uint32_t qty) {
    ecs_entity_t e = entity_spawn(EKIND_ITEM);

    Item *d = ecs_get_mut(world_ecs(), e, Item);
    *d = (Item){
        .kind = item_fix_kind(kind),
        .quantity = qty,
        .merger_time = 0,
    };

    return (uint64_t)e;
}

item_id item_find(asset_id kind) {
    for (item_id i=0; i<ITEMS_COUNT; i++) {
        if (items[i].kind == kind)
            return item_resolve_proxy(i);
    }
    return ASSET_INVALID;
}

Item *item_get_data(uint64_t ent) {
    return ecs_get_mut(world_ecs(), ent, Item);
}

void item_use(ecs_world_t *ecs, ItemSlot *it, Position p, uint64_t udata) {
    (void)ecs;
    Item *d = item_get_data(it->ent);
    uint16_t it_id = d->kind;
    item_desc *desc = &items[it_id];
    if (it->ent == 0) return;
    switch (item_get_usage(it_id)) {
        case UKIND_HOLD: /* NOOP */ break;
        case UKIND_PLACE:{
            world_block_lookup l = world_block_from_realpos(p.x, p.y);
            if (l.is_outer && l.bid > 0) {
                asset_id item_asset = blocks_get_asset(l.bid);
                item_id item_asset_id = item_find(item_asset);
                if (item_asset_id == ASSET_INVALID) return;

                // NOTE(zaklaus): If we replace the same item, refund 1 qty and let it replace it
                if (item_asset_id == it_id) {
                    d->quantity++;
                } else {
                    return;
                }
            }
            // NOTE(zaklaus): This is an inner layer block, we can't build over it if it has a collision!
            else if (l.bid > 0 && blocks_get_flags(l.bid) & (BLOCK_FLAG_COLLISION|BLOCK_FLAG_ESSENTIAL)) {
                return;
            }
            world_chunk_replace_block(l.chunk_id, l.id, blocks_find(desc->place.kind + (asset_id)udata));
            d->quantity--;
        }break;

        case UKIND_PLACE_ITEM:{
            world_block_lookup l = world_block_from_realpos(p.x, p.y);
            if (l.is_outer && l.bid > 0) {
                return;
            }
            // NOTE(zaklaus): This is an inner layer block, we can't build over it if it has a collision!
            else if (l.bid > 0 && blocks_get_flags(l.bid) & (BLOCK_FLAG_COLLISION|BLOCK_FLAG_ESSENTIAL)) {
                return;
            }

            ecs_entity_t e = entity_spawn_id(desc->place_item.id);
            ZPL_ASSERT(world_entity_valid(e));
            entity_set_position(e, p.x, p.y);

            d->quantity--;
        }break;


        case UKIND_DELETE:
        case UKIND_END_PLACE:
        case UKIND_PROXY:
        break;
    }
}

void item_despawn(uint64_t id) {
    entity_despawn(id);
}

uint32_t item_max_quantity(item_id id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].max_quantity;
}

item_usage item_get_usage(item_id id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].usage;
}

bool item_get_place_directional(item_id id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].place.directional;
}
