#include "items.h"
#include "entity.h"
#include "entity_view.h"
#include "world/world.h"
#include "world/blocks.h"

#include "modules/components.h"

#include "zpl.h"

#include "items_list.c"
#define ITEMS_COUNT (sizeof(items)/sizeof(item_desc))

uint64_t item_spawn(item_kind kind, uint32_t qty) {
    ecs_entity_t e = entity_spawn(EKIND_ITEM);
    
    ItemDrop *d = ecs_get_mut(world_ecs(), e, ItemDrop, NULL);
    *d = (ItemDrop){
        .kind = kind,
        .quantity = qty,
        .merger_time = 0,
    };
    
    return (uint64_t)e;
}

uint16_t item_find(item_kind kind) {
    for (uint32_t i=0; i<ITEMS_COUNT; i++) {
        if (items[i].kind == kind)
            return i;
    }
    return ITEMS_INVALID;
}

void item_use(ecs_world_t *ecs, ItemDrop *it, Position p) {
    (void)ecs;
    uint16_t item_id = item_find(it->kind);
    item_desc *desc = &items[item_id];
    if (it->quantity <= 0) return;
    switch (item_get_usage(item_id)) {
        case UKIND_PLACE:{
            world_block_lookup l = world_block_from_realpos(p.x, p.y);
            if (world_chunk_place_block(l.chunk_id, l.id, blocks_find(desc->place.biome, desc->place.kind)) )
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

asset_id item_get_asset(uint16_t id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].asset;
}

item_usage item_get_usage(uint16_t id) {
    ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
    return items[id].usage;
}