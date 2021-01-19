#include "components/controllers.h"

#include "components/general.h"
#include "components/physics.h"

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

    ECS_PREFAB(ecs, Base, general.Position, physics.Velocity, Input, EcsActor);
    ECS_TYPE(ecs, Player, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Walking, EcsActor, EcsPlayer);
    ECS_TYPE(ecs, Builder, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Flying, EcsActor, EcsBuilder);

    ECS_SET_COMPONENT(Input);
    ECS_SET_ENTITY(EcsActor);
    ECS_SET_ENTITY(EcsPlayer);
    ECS_SET_ENTITY(EcsBuilder);
    ECS_SET_TYPE(Builder);
    ECS_SET_TYPE(Player);
}
