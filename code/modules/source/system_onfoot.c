#define PLR_MOVE_SPEED 50.0
#define PLR_MOVE_SPEED_MULT 4.0

void MovementImpulse(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    Position *p = ecs_term(it, Position, 3);
    
    for (int i = 0; i < it->count; i++) {
        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = zpl_clamp(blocks_get_drag(lookup.block_id), 0.0f, 1.0f);
        double speed = PLR_MOVE_SPEED * (in[i].sprint ? PLR_MOVE_SPEED_MULT : 1.0);
        if (zpl_abs(v[i].x) < speed && in[i].x)
            v[i].x += in[i].x*speed*drag;
        if (zpl_abs(v[i].y) < speed && in[i].y)
            v[i].y += in[i].y*speed*drag;
    }
}
