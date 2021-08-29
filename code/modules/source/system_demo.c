
#define DEMO_NPC_MOVE_SPEED 50
#define DEMO_NPC_STEER_SPEED 30

void DemoNPCMoveAround(ecs_iter_t *it) {
    Velocity *v = ecs_column(it, Velocity, 1);
    for (int i = 0; i < it->count; i++) {
        float d = zpl_quake_rsqrt(v[i].x*v[i].x + v[i].y*v[i].y);
        v[i].x += (v[i].x*d*DEMO_NPC_MOVE_SPEED + zpl_cos(zpl_to_radians(rand()%360))*DEMO_NPC_STEER_SPEED);
        v[i].y += (v[i].y*d*DEMO_NPC_MOVE_SPEED + zpl_sin(zpl_to_radians(rand()%360))*DEMO_NPC_STEER_SPEED);
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
