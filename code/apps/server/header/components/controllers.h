#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

#include "components/general.h"
#include "components/physics.h"

ECS_STRUCT(Input, {
    double x;
    double y;
    uint8_t use;
});

typedef struct {
    ECS_DECLARE_COMPONENT(Input);
    ECS_DECLARE_ENTITY(EcsActor);
    ECS_DECLARE_ENTITY(EcsPlayer);
    ECS_DECLARE_ENTITY(EcsBuilder);
    ECS_DECLARE_TYPE(Player);
    ECS_DECLARE_TYPE(Builder);
} Controllers;

#define ControllersImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Input);\
    ECS_IMPORT_TYPE(handles, Player);\
    ECS_IMPORT_TYPE(handles, Builder);\
    ECS_IMPORT_ENTITY(handles, EcsActor);\
    ECS_IMPORT_ENTITY(handles, EcsPlayer);\
    ECS_IMPORT_ENTITY(handles, EcsBuilder);\

static inline void ControllersImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Controllers);
    ecs_set_name_prefix(ecs, "Controllers");

    ECS_IMPORT(ecs, General);
    ECS_IMPORT(ecs, Physics);

    ECS_IMPORT(ecs, FlecsMeta);
    ECS_META(ecs, Input);

    ECS_TAG(ecs, EcsActor);
    ECS_TAG(ecs, EcsPlayer);
    ECS_TAG(ecs, EcsBuilder);

    ECS_PREFAB(ecs, Base, general.Position, physics.Velocity, Input);
    ECS_TYPE(ecs, Player, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Walking, EcsActor, EcsPlayer);
    ECS_TYPE(ecs, Builder, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Flying, EcsActor, EcsBuilder);

    ECS_SET_COMPONENT(Input);
    ECS_SET_ENTITY(EcsActor);
    ECS_SET_ENTITY(EcsPlayer);
    ECS_SET_ENTITY(EcsBuilder);
    ECS_SET_TYPE(Builder);
    ECS_SET_TYPE(Player);
}
