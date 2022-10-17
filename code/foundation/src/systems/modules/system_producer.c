void ProduceItems(ecs_iter_t *it) {
    ItemContainer *storage = ecs_field(it, ItemContainer, 1);
    Producer *producer = ecs_field(it, Producer, 2);
    Position *p = ecs_field(it, Position, 3);
    Device *d = ecs_field(it, Device, 4);
    
    for (int i = 0; i < it->count; i++) {
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
                    entity_set_position(e, p[i].x, p[i].y);
                    producer[i].processed_item = 0;
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

