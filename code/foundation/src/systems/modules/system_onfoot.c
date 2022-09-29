#include "models/entity.h"

void MovementImpulse(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);
    Velocity *v = ecs_field(it, Velocity, 2);
    Position *p = ecs_field(it, Position, 3);

    for (int i = 0; i < it->count; i++) {
        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = zpl_clamp(blocks_get_drag(lookup.bid), 0.0f, 1.0f);
        float speed = game_rules.plr_move_speed * (in[i].sprint ? game_rules.plr_move_speed_mult : 1.0f);
        v[i].x += in[i].x*speed*drag*safe_dt(it);
        v[i].y -= in[i].y*speed*drag*safe_dt(it);

        if (   zpl_abs(v[i].x) > ENTITY_ACTION_VELOCITY_THRESHOLD
            || zpl_abs(v[i].y) > ENTITY_ACTION_VELOCITY_THRESHOLD) {
            entity_wake(it->entities[i]);
        }
    }
}
