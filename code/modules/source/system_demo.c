
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
