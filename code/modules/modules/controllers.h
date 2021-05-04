#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

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

void ControllersImport(ecs_world_t *ecs);
