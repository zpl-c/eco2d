#include "models/items.h"
#include "models/entity.h"
#include "world/entity_view.h"
#include "world/world.h"
#include "world/blocks.h"

#include "models/components.h"

#include "zpl.h"

#include "lists/items_list.c"
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
    librg_entity_visibility_global_set(world_tracker(), ent, show ? LIBRG_VISIBLITY_DEFAULT : LIBRG_VISIBLITY_NEVER);
    c->id = show ? EKIND_ITEM : EKIND_SERVER;
}

uint64_t item_spawn(asset_id kind, uint32_t qty) {
    ecs_entity_t e = entity_spawn(EKIND_ITEM);
    
    Item *d = ecs_get_mut(world_ecs(), e, Item);
    *d = (Item){
        .kind = item_fix_kind(kind),
        .quantity = qty,
        .merger_time = 0,
        .durability = 1.0f,
    };
    
    item_desc *it = &items[item_find(kind)];
    
    if (it->has_storage) {
        ecs_add(world_ecs(), e, BlockHarvest);
        ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
        *storage = (ItemContainer){0};
    }
    
    switch (it->attachment) {
        case UDATA_ENERGY_SOURCE: {
            EnergySource *f = ecs_get_mut(world_ecs(), e, EnergySource);
            *f = (EnergySource){
                .kind = it->energy_source.producer,
                .energy_level = it->energy_source.energy_level,
            };
        } break;
        default: break;
    }
    
    return (uint64_t)e;
}

item_id item_find(asset_id kind) {
    for (item_id i=0; i<ITEMS_COUNT; i++) {
        if (items[i].kind == kind)
            return item_resolve_proxy(i);
    }
    return ASSET_INVALID;
}

item_id item_find_no_proxy(asset_id kind) {
    for (item_id i=0; i<ITEMS_COUNT; i++) {
        if (items[i].kind == kind)
            return i;
    }
    return ASSET_INVALID;
}

Item *item_get_data(uint64_t ent) {
    if (!world_entity_valid(ent)) return NULL;
    return ecs_get_mut_if_ex(world_ecs(), ent, Item);
}

const Item *item_get_data_const(uint64_t ent) {
    if (!world_entity_valid(ent)) return NULL;
    return ecs_get(world_ecs(), ent, Item);
}

void item_use(ecs_world_t *ecs, ecs_entity_t e, Item *it, Position p, uint64_t udata) {
    (void)ecs;
    if (e == 0) return;
    uint16_t it_id = item_find(it->kind);
    item_desc *desc = &items[it_id];
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
                    it->quantity++;
                } else {
                    return;
                }
            }
            // NOTE(zaklaus): This is an inner layer block, we can't build over it if it has a collision!
            else if (l.bid > 0 && blocks_get_flags(l.bid) & (BLOCK_FLAG_COLLISION|BLOCK_FLAG_ESSENTIAL)) {
                return;
            }
            world_chunk_replace_block(l.chunk_id, l.id, blocks_find(desc->place.kind + (asset_id)udata));
            it->quantity--;
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
            
            it->quantity--;
        }break;
        
        case UKIND_PLACE_ITEM_DATA:{
            world_block_lookup l = world_block_from_realpos(p.x, p.y);
            if (l.is_outer && l.bid > 0) {
                return;
            }
            // NOTE(zaklaus): This is an inner layer block, we can't build over it if it has a collision!
            else if (l.bid > 0 && blocks_get_flags(l.bid) & (BLOCK_FLAG_COLLISION|BLOCK_FLAG_ESSENTIAL)) {
                return;
            }
            
            ecs_entity_t e = entity_spawn_id_with_data(desc->place_item.id, desc);
            ZPL_ASSERT(world_entity_valid(e));
            entity_set_position(e, p.x, p.y);
            
            it->quantity--;
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

item_desc item_get_desc(item_id id) {
	ZPL_ASSERT(id >= 0 && id < ITEMS_COUNT);
	return items[id];
}
