#include "models/crafting.h"

static inline
asset_id FetchAssetAtPos(float x, float y) {
    world_block_lookup lookup = world_block_from_realpos(x, y);
    if (lookup.is_outer) {
        return blocks_get_asset(lookup.bid);
    }
    
    return ASSET_INVALID;
}


// NOTE(zaklaus): Expects float[4] dx, dy
static inline
uint8_t CheckForNearbyBelts(Position *p, float *dx, float *dy) {
    asset_id bid = ASSET_INVALID;
    float o = WORLD_BLOCK_SIZE;
    uint8_t nodes = 0x0;
    
    // up
    bid = FetchAssetAtPos(p->x + 0, p->y - o);
    if (bid == ASSET_BELT_UP) {
        dx[0] = 0;
        dy[0] = -o;
        nodes |= ZPL_BIT(0);
    }
    
    // down
    bid = FetchAssetAtPos(p->x + 0, p->y + o);
    if (bid == ASSET_BELT_DOWN) {
        dx[1] = 0;
        dy[1] = o;
        nodes |= ZPL_BIT(1);
    }
    
    // left
    bid = FetchAssetAtPos(p->x - o, p->y + 0);
    if (bid == ASSET_BELT_LEFT) {
        dx[2] = -o;
        dy[2] = 0;
        nodes |= ZPL_BIT(2);
    }
    
    // right
    bid = FetchAssetAtPos(p->x + o, p->y + 0);
    if (bid == ASSET_BELT_RIGHT) {
        dx[3] = o;
        dy[3] = 0;
        nodes |= ZPL_BIT(3);
    }
    
    return nodes;
}

void PushItemsOnNodes(ecs_iter_t *it) {
    ItemContainer *storage = ecs_field(it, ItemContainer, 1);
    Position *p = ecs_field(it, Position, 2);
    Device *d = ecs_field(it, Device, 3);
    ItemRouter *r = ecs_field(it, ItemRouter, 4);
    
    for (int i = 0; i < it->count; i++) {
        // TODO(zaklaus): Cache output nodes so we avoid
        // calling world_block_from_realpos each time.
        // We need a way to refer to specific blocks in the world so we can do easy block ID checks
        // and re-build the cache when a change is detected.
        
        Producer *producer = ecs_get_mut_if_ex(it->world, it->entities[i], Producer);
        
        if (producer) {
            if (producer->push_filter == PRODUCER_PUSH_NONE)
                continue;
        }
        
        float push_dx[4], push_dy[4];
        uint8_t nodes = CheckForNearbyBelts(&p[i], push_dx, push_dy);
        uint8_t num_nodes = (uint8_t)zpl_count_set_bits(nodes);
        
        if (num_nodes == 0) {
            // NOTE(zaklaus): We don't have any output nodes yet.
            continue;
        }
        
        for (int j = 0; j < ITEMS_CONTAINER_SIZE; j++) {
            ecs_entity_t item_slot_ent = storage[i].items[j];
            if (item_slot_ent == 0) continue;
            Item *item = item_get_data(item_slot_ent);
            if (!item) continue;
            
            if (producer) {
                if (producer->push_filter == PRODUCER_PUSH_ANY) {
                    if (craft_is_reagent_used_in_producer(item->kind, d[i].asset)) {
                        // NOTE(zaklaus): this is an input reagent, keep it
                        continue;
                    }
                }
                else if (producer->push_filter == PRODUCER_PUSH_PRODUCT) {
                    if (producer->target_item != item->kind) {
                        // NOTE(zaklaus): this is not a producer's output, keep it
                        continue;
                    }
                }
            }
            
            while (item->quantity > 0 && num_nodes > 0) {
                // NOTE(zaklaus): Use a rolling counter to select an output node.
                while (!(nodes & (1 << r[i].counter)) && r[i].counter < 4) ++r[i].counter;
                if (r[i].counter > 3) {
                    r[i].counter = 0;
                    continue;
                }
                
                uint64_t e = item_spawn(item->kind, zpl_min(r->push_qty, item->quantity));
                entity_set_position(e, p[i].x + push_dx[r[i].counter], p[i].y + push_dy[r[i].counter]);
                
                Velocity *e_vel = ecs_get_mut_ex(it->world, e, Velocity);
                e_vel->x = push_dx[r[i].counter];
                e_vel->y = push_dy[r[i].counter];
                
                item->quantity -= zpl_min(r->push_qty, item->quantity);
                --num_nodes;
                ++r[i].counter;
                
                if (item->quantity == 0) {
                    item_despawn(item_slot_ent);
                    storage[i].items[j] = 0;
                }
            }
        }
    }
}