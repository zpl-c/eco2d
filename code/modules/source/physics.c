#include "modules/physics.h"
#include "world/world.h"
#include "zpl.h"

#define PHY_WALK_DRAG 0.02

void MoveWalk(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);

    for (int i = 0; i < it->count; i++) {
        // TODO: handle collisions
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
        v[i].x = zpl_lerp(v[i].x, 0.0f, PHY_WALK_DRAG);
        v[i].y = zpl_lerp(v[i].y, 0.0f, PHY_WALK_DRAG);
    }
}

void HandleCollisions(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    
    for (int i = 0; i < it->count; i++) {
        // NOTE(zaklaus): world bounds
        {
            double w = (double)world_dim()/2.0;
            p[i].x = zpl_clamp(p[i].x, -w, w);
            p[i].y = zpl_clamp(p[i].y, -w, w);
        }
    }
}

void UpdateTrackerPos(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    
    for (int i = 0; i < it->count; i++){
        librg_entity_chunk_set(world_tracker(), it->entities[i], librg_chunk_from_realpos(world_tracker(), p[i].x, p[i].y, 0));
    }
}

void PhysicsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Physics);
    ecs_set_name_prefix(ecs, "Physics");
    
    ECS_TAG(ecs, Walking);
    ECS_TAG(ecs, Flying);
    ECS_TYPE(ecs, Movement, Walking, Flying);

    ECS_META(ecs, Velocity);

    ECS_SYSTEM(ecs, MoveWalk, EcsOnUpdate, general.Position, Velocity);
    ECS_SYSTEM(ecs, HandleCollisions, EcsOnValidate, general.Position, Velocity);
    ECS_SYSTEM(ecs, UpdateTrackerPos, EcsPostUpdate, general.Position);

    ECS_SET_TYPE(Movement);
    ECS_SET_ENTITY(Walking);
    ECS_SET_ENTITY(Flying);
    ECS_SET_COMPONENT(Velocity);
    ECS_SET_ENTITY(MoveWalk);
    ECS_SET_ENTITY(UpdateTrackerPos);
    ECS_SET_ENTITY(HandleCollisions);
}
