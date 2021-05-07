#include "modules/controllers.h"

#include "modules/general.h"
#include "modules/physics.h"
#include "zpl.h"

// TODO(zaklaus): move to physics
#define CTRL_DRAG_FACTOR 0.20

void MovementImpulse(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    
    for (int i = 0; i < it->count; i++) {
        v[i].x = zpl_lerp(v[i].x, in[i].x*1000, CTRL_DRAG_FACTOR);
        v[i].y = zpl_lerp(v[i].y, in[i].y*1000, CTRL_DRAG_FACTOR);
    }
}

void ControllersImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Controllers);
    ecs_set_name_prefix(ecs, "Controllers");

    ECS_IMPORT(ecs, General);
    ECS_IMPORT(ecs, Physics);
    ECS_IMPORT(ecs, FlecsMeta);

    ECS_META(ecs, Input);

    ECS_TAG(ecs, EcsActor);
    ECS_TAG(ecs, EcsPlayer);
    ECS_TAG(ecs, EcsBuilder);
    
    ECS_SYSTEM(ecs, MovementImpulse, EcsOnUpdate, Input, physics.Velocity);

    ECS_PREFAB(ecs, Base, general.Position, physics.Velocity, Input, EcsActor);
    ECS_TYPE(ecs, Player, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Walking, EcsActor, EcsPlayer);
    ECS_TYPE(ecs, Builder, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Flying, EcsActor, EcsBuilder);

    ECS_SET_COMPONENT(Input);
    ECS_SET_ENTITY(EcsActor);
    ECS_SET_ENTITY(EcsPlayer);
    ECS_SET_ENTITY(EcsBuilder);
    ECS_SET_TYPE(Builder);
    ECS_SET_TYPE(Player);
    ECS_SET_ENTITY(MovementImpulse);
}
