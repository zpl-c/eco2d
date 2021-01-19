#pragma once
#include "flecs/flecs.h"

#include "modules/general.h"

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

void PhysicsImport(ecs_world_t *ecs);
