void FurnaceCook(ecs_iter_t *it) {
    ItemContainer *storage = ecs_field(it, ItemContainer, 1);
    Furnace *furnace = ecs_field(it, Furnace, 2);
    Position *p = ecs_field(it, Position, 3);
    Device *d = ecs_field(it, Device, 4);

    for (int i = 0; i < it->count; i++) {
        for (int j = 0; j < ITEMS_CONTAINER_SIZE; j++) {
            ecs_entity_t item_slot_ent = storage[i].items[j];
            Item *item = item_get_data(item_slot_ent);

            const Fuel *fuel = 0;
            if ((fuel = ecs_get(it->world, item_slot_ent, Fuel))) {
                if (fuel->kind == d->asset) {
                    furnace[i].burn_time += fuel->kind;
                    item_despawn(item_slot_ent);
                    storage[i].items[j] = 0;
                }
                continue;
            }

            // if (furnace[i].burn_time <= 0.0f) continue; TODO
            if (furnace[i].cook_time < game_time()) {
                if (furnace[i].processed_item > 0) {
                    uint64_t e = item_spawn(furnace[i].processed_item, 1);
                    entity_set_position(e, p[i].x, p[i].y);
                    furnace[i].processed_item = 0;
                } else {
                    const Ingredient *ing = 0;
                    if ((ing = ecs_get(it->world, item_slot_ent, Ingredient))) {
                        if (ing->producer == d->asset) {
                            furnace[i].processed_item = ing->product;
                            furnace[i].cook_time = game_time() + game_rules.furnace_cook_time;
                            zpl_printf("e_id %llu, qty: %d\n", item_slot_ent, item->quantity);
                            item->quantity--;
                            if (item->quantity <= 0) {
                                item_despawn(item_slot_ent);
                                storage[i].items[j] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

