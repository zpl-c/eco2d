#include "ents/items.h"

void PickItem(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 2);
    Inventory *inv = ecs_field(it, Inventory, 3);

    for (int i = 0; i < it->count; i++) {
        if (inv[i].pickup_time > game_time()) continue;
        size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);
        bool picked = false;

        for (size_t j = 0; j < ents_count; j++) {
            Item *drop = 0;
            uint64_t ent_id = ents[j];
            if ((drop = ecs_get_mut_if_ex(it->world, ent_id, Item))) {
                Position *p2 = ecs_get_mut_ex(it->world, ent_id, Position);
                Velocity *v2 = ecs_get_mut_ex(it->world, ent_id, Velocity);

                float dx = p2->x - p[i].x;
                float dy = p2->y - p[i].y;
                float range = zpl_sqrt(dx*dx + dy*dy);
                if (range <= game_rules.item_pick_radius) {
                    uint16_t drop_id = item_find(drop->kind);
                    for (size_t k = 0; k < ITEMS_INVENTORY_SIZE; k += 1) {
                        ecs_entity_t item_slot_ent = inv[i].items[k];
                        Item *item = item_get_data(item_slot_ent);
                        uint16_t item_id = item ? item_find(item->kind) : 0;
                        if (!item || (item_id != ASSET_INVALID && (item->kind == drop->kind && item->durability == drop->durability) && item->quantity < item_max_quantity(drop_id))) {
                            if (item) {
                                uint32_t picked_count = zpl_max(0, drop->quantity);
                                picked_count = zpl_clamp(picked_count, 0, item_max_quantity(drop_id) - item->quantity);
                                item->quantity += picked_count;
                                drop->quantity -= picked_count;
                                item->kind = drop->kind;

                                if (drop->quantity == 0)
                                    item_despawn(ent_id);
                            } else if (!world_entity_valid(item_slot_ent)) {
                                entity_wake(ent_id);
                                item_show(ent_id, false);
                                inv[i].items[k] = ent_id;
                            }
                            picked = true;
                            break;
                        }
                    }
                } else if (range <= game_rules.item_attract_radius) {
                    v2->x = (p[i].x - p2->x) * game_rules.item_attract_force;
                    v2->y = (p[i].y - p2->y) * game_rules.item_attract_force;
                }
            }

            if (picked) break;
        }
    }
}

void DropItem(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);
    Position *p = ecs_field(it, Position, 2);
    Inventory *inv = ecs_field(it, Inventory, 3);

    for (int i = 0; i < it->count; i++) {
        if (!in[i].drop) continue;

        ecs_entity_t *items = inv[i].items;

        if (in[i].storage_action){
            if (world_entity_valid(in[i].storage_ent)){
                ItemContainer *ic = 0;
                if ((ic = ecs_get_mut_if_ex(it->world, in[i].storage_ent, ItemContainer))){
                    items = ic->items;
                }else{
                    continue;
                }
            }else{
                continue;
            }
        }

        uint8_t slot_id = in[i].storage_action ? in[i].storage_selected_item : in[i].selected_item;
        ecs_entity_t item_slot_ent = items[slot_id];
        Item *item = item_get_data(item_slot_ent);

        if (!item || item->quantity <= 0)
            continue;

        uint32_t dropped_count = item->quantity;
        if (in[i].sprint) {
            dropped_count /= 2;
        } else if (in[i].ctrl) {
            dropped_count = item->quantity-1;
        }

        if (dropped_count == 0)
            continue;

        item_show(item_slot_ent, true);

        Position *ipos = ecs_get_mut_ex(it->world, item_slot_ent, Position);
        entity_set_position(item_slot_ent, p[i].x, p[i].y);

        Velocity *v = ecs_get_mut_ex(it->world, item_slot_ent, Velocity);
        v->x = in[i].mx * 800.0f;
        v->y = in[i].my * 800.0f;

        inv[i].pickup_time = game_time() + game_rules.item_drop_pickup_time;
        in[i].drop = false;
        items[slot_id] = 0;

        if (item->quantity - dropped_count > 0) {
            item->quantity -= dropped_count;
            ecs_entity_t te = item_spawn(item->kind, dropped_count);
            item_show(te, false);
            items[slot_id] = te;
        }
    }
}

// void MergeItems(ecs_iter_t *it) {
//     Position *p = ecs_field(it, Position, 1);
//     ItemDrop *id = ecs_field(it, ItemDrop, 2);

//     for (int i = 0; i < it->count; i += 1) {
//         ItemDrop *item = &id[i];

//         if (item->merger_time < game_time())
//             continue;

//         size_t ents_count;
//         int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 1);

//         for (size_t j = 0; j < ents_count; j++) {
//             ItemDrop *drop = 0;
//             if ((drop = ecs_get_mut_if_ex(it->world, ents[j], ItemDrop))) {
//                 if (drop->kind != item->kind || (ecs_entity_t)ents[j] == it->entities[i] || drop->quantity == 0 || item->quantity == 0)
//                     continue;

//                 Position const* p2 = ecs_get(it->world, ents[j], Position);

//                 float dx = p2->x - (p[i].x);
//                 float dy = p2->y - (p[i].y);
//                 float range = zpl_sqrt(dx*dx + dy*dy);
//                 if (range <= game_rules.item_merger_radius) {
//                     drop->quantity += item->quantity;
//                     item_despawn(it->entities[i]);
//                     break;
//                 }
//             }
//         }
//     }
// }

void SwapItems(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);
    Inventory *inv = ecs_field(it, Inventory, 2);

    for (int i = 0; i < it->count; i++) {
        if (!in[i].swap) continue;

        ecs_entity_t *items = inv[i].items;

        if (in[i].storage_action){
            if (world_entity_valid(in[i].storage_ent)){
                ItemContainer *ic = 0;
                if ((ic = ecs_get_mut_if_ex(it->world, in[i].storage_ent, ItemContainer))){
                    items = ic->items;
                }else{
                    continue;
                }
            }else{
                continue;
            }
        }

        ecs_entity_t *from_ent = 0;
        ecs_entity_t *to_ent = 0;
        Item *to = 0;
        Item *from = 0;

        if (in[i].swap_storage){
            in[i].swap_storage = false;

            if (in[i].storage_action){
                from_ent = &inv[i].items[in[i].swap_from];
                to_ent = &items[in[i].swap_to];
                from = item_get_data(*from_ent);
                to = item_get_data(*to_ent);
            }else{
                if (world_entity_valid(in[i].storage_ent)){
                    ItemContainer *ic = 0;
                    if ((ic = ecs_get_mut_if_ex(it->world, in[i].storage_ent, ItemContainer))){
                        from_ent = &ic->items[in[i].swap_from];
                        from = item_get_data(*from_ent);
                    }else{
                        continue;
                    }
                }else{
                    continue;
                }
                to_ent = &items[in[i].swap_to];
                to = item_get_data(*to_ent);
            }
        }else{
            from_ent = &items[in[i].swap_from];
            to_ent = &items[in[i].swap_to];
            from = item_get_data(*from_ent);
            to = item_get_data(*to_ent);
        }

        if (!from) continue;

        uint16_t to_id = to ? item_find(to->kind) : ASSET_EMPTY;
        asset_id to_kind = to ? to->kind : ASSET_EMPTY;

        if (to_ent == from_ent) {
            // NOTE(zaklaus): do nothing
        } else if (to_kind == from->kind && to->quantity > 0) {
            uint32_t swapped_count = from->quantity;
            if (in[i].sprint) {
                swapped_count /= 2;
            } else if (in[i].ctrl) {
                swapped_count = 1;
            }
            swapped_count = zpl_clamp(swapped_count, 0, item_max_quantity(to_id) - to->quantity);
            to->quantity += swapped_count;
            from->quantity -= swapped_count;

            if (from->quantity == 0) {
                item_despawn(*from_ent);
                *from_ent = 0;
            }
        } else if ((in[i].ctrl || in[i].sprint) && to == 0 && from->quantity > 0) {
            // NOTE(zaklaus): item split
            uint32_t split_count = from->quantity / 2;
            if (in[i].ctrl) {
                split_count = 1;
            }
            if (from->quantity - split_count == 0) {
                continue;
            }
            ecs_entity_t te = item_spawn(from->kind, split_count);
            item_show(te, false);
            *to_ent = te;

            from->quantity -= split_count;
        } else {
            ecs_entity_t tmp = *to_ent;
            *to_ent = *from_ent;
            *from_ent = tmp;
        }

        in[i].swap = false;
    }
}

void UseItem(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);
    Position *p = ecs_field(it, Position, 2);
    Inventory *inv = ecs_field(it, Inventory, 3);

    for (int i = 0; i < it->count; i++) {
        if (!in[i].use && !in[i].num_placements) continue;

        if (in[i].storage_action){
            continue;
        }

        ecs_entity_t item_ent = inv[i].items[in[i].selected_item];
        Item *item = item_get_data(item_ent);
        uint16_t item_id = 0;
        item_usage usage = UKIND_DELETE;

        if (!in[i].deletion_mode){
            item_id = item ? item_find(item->kind) : ASSET_EMPTY;
            usage = item_get_usage(item_id);
            if (!item || item->quantity <= 0) continue;
        }

        if (!in[i].use && usage == UKIND_DELETE){
            for (size_t j = 0; j < in[i].num_placements; j++) {
                world_chunk_destroy_block(in[i].placements_x[j], in[i].placements_y[j], true);
            }
        }
        else if (in[i].use && usage > UKIND_END_PLACE)
            item_use(it->world, item_ent, item, p[i], 0);
        else if (in[i].num_placements > 0 && usage < UKIND_END_PLACE) {
            asset_id ofs = 0;
            if (item_get_place_directional(item_id) && in[i].num_placements >= 2) {
                float p1x = in[i].placements_x[0];
                float p1y = in[i].placements_y[0];
                float p2x = in[i].placements_x[1];
                float p2y = in[i].placements_y[1];
                float sx = zpl_sign0(p2x-p1x);
                float sy = zpl_sign0(p2y-p1y);
                ofs = (sx < 0.0f) ? 1 : 2;
                if (sx == 0.0f) {
                    ofs = (sy < 0.0f) ? 3 : 4;
                }
            } else if(item_get_place_directional(item_id)) {
                // NOTE(zaklaus): ensure we pick the first variant
                ofs = 1;
            }

            for (size_t j = 0; j < in[i].num_placements; j++) {
                Position pos = {.x = in[i].placements_x[j], .y = in[i].placements_y[j]};
                item_use(it->world, item_ent, item, pos, ofs);
            }

            in[i].num_placements = 0;
        }

        entity_wake(it->entities[i]);

        if (item->quantity == 0) {
            item_despawn(item_ent);
            inv[i].items[in[i].selected_item] = 0;
        }
    }
}

void InspectContainers(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);

    for (int i = 0; i < it->count; ++i) {
        if (!in[i].pick) continue;

        if ((in[i].sel_ent && ecs_get(it->world, in[i].sel_ent, ItemContainer)) || !in[i].sel_ent)
            in[i].storage_ent = in[i].sel_ent;
    }
}

void HarvestIntoContainers(ecs_iter_t *it) {
    ItemContainer *in = ecs_field(it, ItemContainer, 1);
    Position *p = ecs_field(it, Position, 2);

    for (int i = 0; i < it->count; ++i) {
        // NOTE(zaklaus): find any item
        size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 0);
        bool picked = false;

        for (size_t j = 0; j < ents_count; j++) {
            Item *drop = 0;
            if ((drop = ecs_get_mut_if_ex(it->world, ents[j], Item))) {
                Position *p2 = ecs_get_mut_ex(it->world, ents[j], Position);
                uint64_t ent_id = ents[j];

                float dx = p2->x - p[i].x;
                float dy = p2->y - p[i].y;
                float range = zpl_sqrt(dx*dx + dy*dy);
                if (range <= game_rules.item_pick_radius) {
                    uint16_t drop_id = item_find(drop->kind);
                    for (size_t k = 0; k < ITEMS_CONTAINER_SIZE; k += 1) {
                        uint64_t item_slot_ent = in[i].items[k];
                        Item *item = item_get_data(item_slot_ent);
                        uint16_t item_id = item ? item_find(item->kind) : 0;
                        if (!item || (item_id != ASSET_INVALID && (item->kind == drop->kind && item->durability == drop->durability) && item->quantity < item_max_quantity(drop_id))) {
                            if (item) {
                                uint32_t picked_count = zpl_max(0, drop->quantity);
                                picked_count = zpl_clamp(picked_count, 0, item_max_quantity(drop_id) - item->quantity);
                                item->quantity += picked_count;
                                drop->quantity -= picked_count;

                                if (drop->quantity == 0) {
                                    item_despawn(ent_id);
                                }
                            } else if (!world_entity_valid(item_slot_ent)) {
                                entity_wake(ent_id);
                                item_show(ent_id, false);
                                in[i].items[k] = ent_id;
                            }
                            entity_wake(it->entities[i]);
                            picked = true;
                            break;
                        }
                    }
                }
            }
            if (picked) break;
        }
    }
}
