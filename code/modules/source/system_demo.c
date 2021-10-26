
#define DEMO_NPC_MOVE_SPEED 500
#define DEMO_NPC_STEER_SPEED 300

void DemoNPCMoveAround(ecs_iter_t *it) {
    Velocity *v = ecs_column(it, Velocity, 1);
    for (int i = 0; i < it->count; i++) {
        float d = zpl_quake_rsqrt(v[i].x*v[i].x + v[i].y*v[i].y);
        v[i].x += (v[i].x*d*DEMO_NPC_MOVE_SPEED*it->delta_time + zpl_cos(zpl_to_radians(rand()%360))*DEMO_NPC_STEER_SPEED*it->delta_time);
        v[i].y += (v[i].y*d*DEMO_NPC_MOVE_SPEED*it->delta_time + zpl_sin(zpl_to_radians(rand()%360))*DEMO_NPC_STEER_SPEED*it->delta_time);
    }
}
