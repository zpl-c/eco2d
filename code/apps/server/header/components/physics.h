#pragma once
#include "flecs/flecs.h"
#include "components/general.h"

typedef Vector2D Velocity;

typedef struct {
    ECS_DECLARE_TYPE(Movement);
    ECS_DECLARE_ENTITY(Walking);
    ECS_DECLARE_ENTITY(Flying);
    ECS_DECLARE_COMPONENT(Velocity);
} Physics;

#define PhysicsImportHandles(handles)\
    ECS_IMPORT_TYPE(handles, Movement);\
    ECS_IMPORT_ENTITY(handles, Walking);\
    ECS_IMPORT_ENTITY(handles, Flying);\
    ECS_IMPORT_COMPONENT(handles, Velocity);\

static inline void PhysicsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Physics);
    ecs_set_name_prefix(ecs, "Physics");

    ECS_TAG(ecs, Walking);
    ECS_TAG(ecs, Flying);
    ECS_TYPE(ecs, Movement, Walking, Flying);

    ECS_COMPONENT(ecs, Velocity);

    ECS_SET_TYPE(Movement);
    ECS_SET_ENTITY(Walking);
    ECS_SET_ENTITY(Flying);
    ECS_SET_COMPONENT(Velocity);
}
