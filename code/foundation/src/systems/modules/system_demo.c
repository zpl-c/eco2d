#include <math.h>

void DemoNPCMoveAround(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 1);
    for (int i = 0; i < it->count; i++) {
        float d = zpl_quake_rsqrt(v[i].x*v[i].x + v[i].y*v[i].y);
        v[i].x += (v[i].x*d*game_rules.demo_npc_move_speed*safe_dt(it) + zpl_cos(zpl_to_radians((float)(rand()%360)))*game_rules.demo_npc_steer_speed*safe_dt(it));
        v[i].y += (v[i].y*d*game_rules.demo_npc_move_speed*safe_dt(it) + zpl_sin(zpl_to_radians((float)(rand()%360)))*game_rules.demo_npc_steer_speed*safe_dt(it));

        entity_wake(it->entities[i]);
    }
}
