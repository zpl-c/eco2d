#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

#include "modules/general.h"

ECS_ALIAS(Vector2D, Velocity);

typedef struct {
    ECS_DECLARE_TYPE(Movement);
    ECS_DECLARE_ENTITY(Walking);
    ECS_DECLARE_ENTITY(Flying);
    ECS_DECLARE_COMPONENT(Velocity);
    ECS_DECLARE_ENTITY(MoveWalk);
    ECS_DECLARE_ENTITY(UpdateTrackerPos);
    ECS_DECLARE_ENTITY(IntegratePositions);
} Physics;

#define PhysicsImportHandles(handles)\
ECS_IMPORT_TYPE(handles, Movement);\
ECS_IMPORT_ENTITY(handles, Walking);\
ECS_IMPORT_ENTITY(handles, Flying);\
ECS_IMPORT_COMPONENT(handles, Velocity);\
ECS_IMPORT_ENTITY(handles, MoveWalk);\
ECS_IMPORT_ENTITY(handles, UpdateTrackerPos);\
ECS_IMPORT_ENTITY(handles, IntegratePositions);\

void PhysicsImport(ecs_world_t *ecs);
