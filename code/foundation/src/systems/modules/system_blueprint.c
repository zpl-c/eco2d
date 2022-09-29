void BuildBlueprints(ecs_iter_t *it) {
    Blueprint *blueprint = ecs_field(it, Blueprint, 1);
    Device *d = ecs_field(it, Device, 2);
    Position *p = ecs_field(it, Position, 3);

    for (int i = 0; i < it->count; i++) {
        // TODO check storage and only build if we have enough resources
        // build blocks over time and show progress bar while building a block

        int w = (int)blueprint[i].w;
        int h = (int)blueprint[i].h;

        for (int y = 0; y < blueprint[i].h; y++) {
            for (int x = 0; x < blueprint[i].w; x++) {
                asset_id c = blueprint[i].plan[y*w + x];
                if (c == ASSET_EMPTY) continue;
                world_block_lookup l = world_block_from_realpos(p[i].x + x * WORLD_BLOCK_SIZE - (w * WORLD_BLOCK_SIZE)/2, p[i].y + y * WORLD_BLOCK_SIZE - (h * WORLD_BLOCK_SIZE)/2);
                world_chunk_place_block(l.chunk_id, l.id, blocks_find(c));
            }
        }

        entity_despawn(it->entities[i]);

        // d[i].progress_active = (producer[i].processed_item > 0);
        // d[i].progress_value = 1.0f-((producer[i].process_time - game_time()) / game_rules.furnace_cook_time);
    }
}
