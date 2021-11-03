#include "items.h"
#include "entity.h"
#include "entity_view.h"
#include "world/world.h"
#include "world/blocks.h"

#include "modules/components.h"

#include "zpl.h"

#include "items_list.c"
#define ITEMS_COUNT (sizeof(items)/sizeof(item_desc))

static inline uint16_t item_resolve_proxy(uint16_t id) {
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

uint64_t item_spawn(asset_id kind, uint32_t qty) {
    ecs_entity_t e = entity_spawn(EKIND_ITEM);
    
    ItemDrop *d = ecs_get_mut(world_ecs(), e, ItemDrop, NULL);
    *d = (ItemDrop){
        .kind = item_fix_kind(kind),
        .quantity = qty,
        .merger_time = 0,
    };
    
    return (uint64_t)e;
}

uint16_t item_find(asset_id kind) {
    for (uint16_t i=0; i<ITEMS_COUNT; i++) {
        if (items[i].kind == kind)
            return item_resolve_proxy(i);
    }
    
    ZPL_PANIC("Unknown asset id: %d\n", kind);
    return ASSET_INVALID;
}

void item_use(ecs_world_t *ecs, ItemDrop *it, Position p, uint64_t udata) {
    (void)ecs;
    uint16_t item_id = item_find(it->kind);
    item_desc *desc = &items[item_id];
    if (it->quantity <= 0) return;
    switch (item_get_usage(item_id)) {
        case UKIND_HOLD: /* NOOP */ break;
        case UKIND_PLACE:{
            world_block_lookup l = world_block_from_realpos(p.x, p.y);
            if (world_chunk_place_block(l.chunk_id, l.id, blocks_find(desc->place.kind + (asset_id)udata)) )
                it->quantity--;
        }break;
    }
}

void item_despawn(uint64_t id) {
    entity_despawn(id);
}

uint32_t item_max_quantity(uint16_t id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].max_quantity;
}

item_usage item_get_usage(uint16_t id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].usage;
}

bool item_get_place_directional(uint16_t id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].place.directional;
}