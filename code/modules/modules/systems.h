#pragma once
#include "flecs/flecs.h"

typedef struct {
    ECS_DECLARE_ENTITY(MoveWalk);
    ECS_DECLARE_ENTITY(UpdateTrackerPos);
    ECS_DECLARE_ENTITY(IntegratePositions);
    ECS_DECLARE_ENTITY(PushOutOverlappingEntities);
} Systems;

#define SystemsImportHandles(handles)\
ECS_IMPORT_ENTITY(handles, MoveWalk);\
ECS_IMPORT_ENTITY(handles, UpdateTrackerPos);\
ECS_IMPORT_ENTITY(handles, IntegratePositions);\
ECS_IMPORT_ENTITY(handles, PushOutOverlappingEntities);\

void SystemsImport(ecs_world_t *ecs);
