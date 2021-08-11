
#define DEMO_NPC_CHANGEDIR_FACTOR 0.1
#define DEMO_NPC_MOVE_SPEED 1500

void DemoNPCMoveAround(ecs_iter_t *it) {
    Velocity *v = ecs_column(it, Velocity, 1);
    for (int i = 0; i < it->count; i++) {
        v[i].x = zpl_lerp(v[i].x, (rand()%3-1)*DEMO_NPC_MOVE_SPEED, DEMO_NPC_CHANGEDIR_FACTOR);
        v[i].y = zpl_lerp(v[i].y, (rand()%3-1)*DEMO_NPC_MOVE_SPEED, DEMO_NPC_CHANGEDIR_FACTOR);
    }
}

void DemoPlaceIceBlock(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Position *p = ecs_column(it, Position, 2);
    uint8_t watr_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);
    
    for (int i = 0; i < it->count; i++) {
        if (in[i].use) {
            in[i].use = false;
            world_block_lookup l = world_block_from_realpos(p[i].x, p[i].y);
            world_chunk_replace_block(it->world, l.chunk_id, l.id, watr_id); 
        }
    }
}
