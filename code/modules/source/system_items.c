#include "items.h"

#define ITEM_PICK_RADIUS 25.0f
#define ITEM_MERGER_RADIUS 75.0f
#define ITEM_ATTRACT_RADIUS 75.0f
#define ITEM_ATTRACT_FORCE 0.63f

void PickItem(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 2);
    Inventory *inv = ecs_column(it, Inventory, 3);
    
    for (int i = 0; i < it->count; i++) {
        if (inv[i].pickup_time > game_time()) continue;
        size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);
        
        for (size_t j = 0; j < ents_count; j++) {
            ItemDrop *drop = 0;
            if ((drop = ecs_get_mut_if(it->world, ents[j], ItemDrop))) {
                Position *p2 = ecs_get_mut(it->world, ents[j], Position, NULL);
                
                float dx = p2->x - p[i].x;
                float dy = p2->y - p[i].y;
                float range = zpl_sqrt(dx*dx + dy*dy);
                if (range <= ITEM_PICK_RADIUS) {
                    for (size_t k = 0; k < ITEMS_INVENTORY_SIZE; k += 1) {
                        ItemDrop *item = &inv[i].items[k];
                        uint16_t item_id = item_find(item->kind);
                        if ((item->quantity == 0 || (item->quantity != 0 && item->kind == drop->kind)) && item->quantity < item_max_quantity(item_id)) {
                            uint32_t picked_count = zpl_max(0, drop->quantity);
                            picked_count = zpl_clamp(picked_count, 0, item_max_quantity(item_id) - item->quantity);
                            item->quantity += picked_count;
                            drop->quantity -= picked_count;
                            
                            if (drop->quantity == 0)
                                item_despawn(ents[j]);
                            break;
                        }
                    }
                } else if (range <= ITEM_ATTRACT_RADIUS) {
                    p2->x = zpl_lerp(p2->x, p[i].x, ITEM_ATTRACT_FORCE*it->delta_time);
                    p2->y = zpl_lerp(p2->y, p[i].y, ITEM_ATTRACT_FORCE*it->delta_time);
                }
            }
        }
    }
}

#define ITEM_DROP_PICKUP_TIME 2.5f
#define ITEM_DROP_MERGER_TIME 6.5f

void DropItem(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Position *p = ecs_column(it, Position, 2);
    Inventory *inv = ecs_column(it, Inventory, 3);
    
    for (int i = 0; i < it->count; i++) {
        if (!in[i].drop) continue;
        
        ItemDrop *item = &inv[i].items[in[i].selected_item];
        
        if (item->quantity <= 0)
            continue;
        
        uint32_t dropped_count = item->quantity;
        if (in[i].sprint) {
            dropped_count /= 2;
        } else if (in[i].ctrl) {
            dropped_count = dropped_count > 0 ? 1 : 0;
        }
        
        if (dropped_count == 0)
            continue;
        
        ecs_entity_t te = item_spawn(item->kind, dropped_count);
        item->quantity -= dropped_count;
        
        ItemDrop *d = ecs_get_mut(world_ecs(), te, ItemDrop, NULL);
        *d = (ItemDrop){
            .kind = item->kind,
            .quantity = dropped_count,
            .merger_time = game_time() + ITEM_DROP_MERGER_TIME,
        };
        
        Position *ipos = ecs_get_mut(it->world, te, Position, NULL);
        *ipos = p[i];
        
        Velocity *v = ecs_get_mut(it->world, te, Velocity, NULL);
        v->x = in[i].mx * 800.0f;
        v->y = in[i].my * 800.0f;
        
        inv[i].pickup_time = game_time() + ITEM_DROP_PICKUP_TIME;
        in[i].drop = false;
    }
}

void MergeItems(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    ItemDrop *id = ecs_column(it, ItemDrop, 2);
    
    for (int i = 0; i < it->count; i += 1) {
        ItemDrop *item = &id[i];
        
        if (item->merger_time < game_time())
            continue;
        
        size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 1);
        
        for (size_t j = 0; j < ents_count; j++) {
            ItemDrop *drop = 0;
            if ((drop = ecs_get_mut_if(it->world, ents[j], ItemDrop))) {
                if (drop->kind != item->kind || (ecs_entity_t)ents[j] == it->entities[i] || drop->quantity == 0 || item->quantity == 0)
                    continue;
                
                Position const* p2 = ecs_get(it->world, ents[j], Position);
                
                float dx = p2->x - (p[i].x);
                float dy = p2->y - (p[i].y);
                float range = zpl_sqrt(dx*dx + dy*dy);
                if (range <= ITEM_MERGER_RADIUS) {
                    drop->quantity += item->quantity;
                    item_despawn(it->entities[i]);
                    break;
                }
            }
        }
    }
}

void SwapItems(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Inventory *inv = ecs_column(it, Inventory, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (!in[i].swap) continue;
        
        ItemDrop *to = &inv[i].items[in[i].swap_to];
        ItemDrop *from = &inv[i].items[in[i].swap_from];
        uint16_t to_id = item_find(to->kind);
        
        if (to == from) {
            // NOTE(zaklaus): do nothing
        } else if (to->kind == from->kind && to->quantity > 0) {
            uint32_t swapped_count = from->quantity;
            if (in[i].sprint) {
                swapped_count /= 2;
            } else if (in[i].ctrl) {
                swapped_count = 1;
            }
            swapped_count = zpl_clamp(swapped_count, 0, item_max_quantity(to_id) - to->quantity);
            to->quantity += swapped_count;
            from->quantity -= swapped_count;
            
            if (swapped_count == 0) {
                ItemDrop tmp = *to;
                *to = *from;
                *from = tmp;
            }
        } else if ((in[i].ctrl || in[i].sprint) && to->quantity == 0 && from->quantity > 0) {
            // NOTE(zaklaus): item split
            uint32_t split_count = from->quantity / 2;
            if (in[i].ctrl) {
                split_count = 1;
            }
            to->quantity = split_count;
            from->quantity -= split_count;
            to->kind = from->kind;
        } else {
            ItemDrop tmp = *to;
            *to = *from;
            *from = tmp;
        }
        
        in[i].swap = false;
    }
}

void UseItem(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Position *p = ecs_column(it, Position, 2);
    Inventory *inv = ecs_column(it, Inventory, 3);
    
    for (int i = 0; i < it->count; i++) {
        if (!in[i].use) continue;
        
        ItemDrop *item = &inv[i].items[in[i].selected_item];
        if (!item || item->quantity <= 0) continue;
        item_use(it->world, item, p[i]);
    }
}