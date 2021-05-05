#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

ECS_STRUCT(ClientInfo, {
    uintptr_t peer;
});

typedef struct {
    ECS_DECLARE_ENTITY(EcsClient);
    ECS_DECLARE_COMPONENT(ClientInfo);
} Net;

#define NetImportHandles(handles)\
    ECS_IMPORT_ENTITY(handles, EcsClient);\
    ECS_IMPORT_COMPONENT(handles, ClientInfo);\

void NetImport(ecs_world_t *ecs);