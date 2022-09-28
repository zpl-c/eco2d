void FurnaceCook(ecs_iter_t *it) {
    ItemContainer *storage = ecs_field(it, ItemContainer, 1);
    Furnace *furnace = ecs_field(it, Furnace, 2);
    Position *p = ecs_field(it, Position, 3);

    for (int i = 0; i < it->count; i++) {
        for (int j = 0; j < ITEMS_CONTAINER_SIZE; j++) {
            if (storage[i].items[j].kind == ASSET_COAL) {
                // furnace[i].fuel_level += itemdb_get(storage[i].items[j].uuid, ITEMDB_FUEL_LEVEL);
                // furnace[i].fuel_level = zpl_clamp(furnace[i].fuel_level + 0.8f, 0.0f, 1.0f);
                storage[i].items[j].quantity--;
                if (storage[i].items[j].quantity == 0) {
                    storage[i].items[j] = (ItemSlot){0};
                }
                continue;
            }

            // if (furnace[i].fuel_level <= 0.0f) continue;
            if (furnace[i].cook_time < game_time()) {
                if (furnace[i].processed_item.kind > 0) {
                    uint64_t e = item_spawn(furnace[i].processed_item.kind, 1);
                    entity_set_position(e, p[i].x, p[i].y);
                    furnace[i].processed_item.kind = 0;
                } else {
                    if (storage[i].items[j].kind == ASSET_DEMO_ICEMAKER) {
                        furnace[i].processed_item.kind = ASSET_BELT;
                        storage[i].items[j].quantity--;
                        if (storage[i].items[j].quantity == 0) {
                            storage[i].items[j] = (ItemSlot){0};
                        }
                        furnace[i].cook_time = game_time() + game_rules.furnace_cook_time;
                    }
                }
            }
        }
    }
}

