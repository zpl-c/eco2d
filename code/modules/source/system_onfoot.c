#define PLR_MOVE_SPEED 30.0
#define PLR_MOVE_SPEED_MULT 1.5

void MovementImpulse(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    Position *p = ecs_column(it, Position, 3);
    
    for (int i = 0; i < it->count; i++) {
        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = zpl_clamp(blocks_get_drag(lookup.block_id), 0.0f, 1.0f);
        double speed = PLR_MOVE_SPEED * (in[i].sprint ? PLR_MOVE_SPEED_MULT : 1.0);
        v[i].x += in[i].x*speed*drag;
        v[i].y += in[i].y*speed*drag;
    }
}
