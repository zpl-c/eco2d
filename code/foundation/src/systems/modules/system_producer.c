static inline
asset_id FetchAssetAtPos(float x, float y) {
    world_block_lookup lookup = world_block_from_realpos(x, y);
    if (lookup.is_outer) {
        return blocks_get_asset(lookup.bid);
    }
    
    return ASSET_INVALID;
}

static inline
bool CheckForNearbyBelt(Position *p, float *dx, float *dy) {
    asset_id bid = ASSET_INVALID;
    float o = WORLD_BLOCK_SIZE;
    
    // up
    bid = FetchAssetAtPos(p->x + 0, p->y - o);
    {
        debug_v2 a = {p->x, p->y};
        debug_v2 b = {p->x, p->y-WORLD_BLOCK_SIZE};
        debug_push_line(a, b, 0xFFFFFFFF);
    }
    if (bid == ASSET_BELT_UP) {
        *dx = 0;
        *dy = -o;
        return true;
    }
    
    // down
    bid = FetchAssetAtPos(p->x + 0, p->y + o);
    if (bid == ASSET_BELT_DOWN) {
        *dx = 0;
        *dy = o;
        return true;
    }
    
    // left
    bid = FetchAssetAtPos(p->x - o, p->y + 0);
    if (bid == ASSET_BELT_LEFT) {
        *dx = -o;
        *dy = 0;
        return true;
    }
    
    // right
    bid = FetchAssetAtPos(p->x + o, p->y + 0);
    if (bid == ASSET_BELT_RIGHT) {
        *dx = o;
        *dy = 0;
        return true;
    }
    
    return false;
}

void ProduceItems(ecs_iter_t *it) {
    ItemContainer *storage = ecs_field(it, ItemContainer, 1);
    Producer *producer = ecs_field(it, Producer, 2);
    Position *p = ecs_field(it, Position, 3);
    Device *d = ecs_field(it, Device, 4);
    
    for (int i = 0; i < it->count; i++) {
        float push_dx=0.0f, push_dy=0.0f;
        bool has_output_node = CheckForNearbyBelt(&p[i], &push_dx, &push_dy);
        
        for (int j = 0; j < ITEMS_CONTAINER_SIZE; j++) {
            ecs_entity_t item_slot_ent = storage[i].items[j];
            Item *item = item_get_data(item_slot_ent);
            
            const EnergySource *energy_source = 0;
            if ((energy_source = ecs_get_if(it->world, item_slot_ent, EnergySource))) {
                if (energy_source->kind == d->asset) {
                    producer[i].energy_level += energy_source->kind;
                    item_despawn(item_slot_ent);
                    storage[i].items[j] = 0;
                }
                continue;
            }
            
            // if (producer[i].energy_level <= 0.0f) continue;
            if (producer[i].process_time < game_time()) {
                if (producer[i].processed_item > 0) {
                    uint64_t e = item_spawn(producer[i].processed_item, 1);
                    producer[i].processed_item = 0;
                    
                    if (has_output_node) {
                        entity_set_position(e, p[i].x+push_dx, p[i].y+push_dy);
                        Velocity *e_vel = ecs_get_mut_ex(it->world, e, Velocity);
                        e_vel->x = push_dx;
                        e_vel->y = push_dy;
                    } else {
                        entity_set_position(e, p[i].x, p[i].y);
                    }
                } else {
                    const Ingredient *ing = 0;
                    if ((ing = ecs_get_if(it->world, item_slot_ent, Ingredient))) {
                        if (ing->producer == d->asset) {
                            if (item->quantity <= 0) {
                                item_despawn(item_slot_ent);
                                storage[i].items[j] = 0;
                            } else {
                                producer[i].processed_item = ing->product;
                                producer[i].process_time = game_time() + game_rules.furnace_cook_time;
                            }
                            item->quantity--;
                        }
                    }
                }
            }
        }
        
        d[i].progress_active = (producer[i].processed_item > 0);
        d[i].progress_value = 1.0f-((producer[i].process_time - game_time()) / game_rules.furnace_cook_time);
        
        if (d[i].progress_active) {
            entity_wake(it->entities[i]);
        }
    }
}

