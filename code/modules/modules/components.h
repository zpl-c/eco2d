#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "world/world.h"

ECS_STRUCT(Vector2D, {
               float x;
               float y;
           });

ECS_STRUCT(Chunk, {
               uint32_t id;
               int16_t x;
               int16_t y;
               uint8_t is_dirty;
           });

ECS_STRUCT(Drawable, {
               uint16_t id;
           });

ECS_ALIAS(Vector2D, Position);
ECS_ALIAS(Vector2D, Velocity);

ECS_STRUCT(Input, {
               float x;
               float y;
               uint8_t use;
               uint8_t sprint;
           });

ECS_STRUCT(ClientInfo, {
               uintptr_t peer;
               uint16_t view_id;                                             
           });

typedef struct {
    ECS_DECLARE_COMPONENT(Chunk);
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_COMPONENT(Vector2D);
    ECS_DECLARE_COMPONENT(Drawable);
    ECS_DECLARE_COMPONENT(Input);
    ECS_DECLARE_ENTITY(EcsActor);
    ECS_DECLARE_ENTITY(EcsPlayer);
    ECS_DECLARE_ENTITY(EcsBuilder);
    ECS_DECLARE_ENTITY(EcsDemoNPC);
    ECS_DECLARE_TYPE(Player);
    ECS_DECLARE_TYPE(Builder);
    ECS_DECLARE_TYPE(Movement);
    ECS_DECLARE_ENTITY(Walking);
    ECS_DECLARE_ENTITY(Flying);
    ECS_DECLARE_COMPONENT(Velocity);
    ECS_DECLARE_ENTITY(EcsClient);
    ECS_DECLARE_COMPONENT(ClientInfo);
} Components;

#define ComponentsImportHandles(handles)\
ECS_IMPORT_COMPONENT(handles, Chunk);\
ECS_IMPORT_COMPONENT(handles, Vector2D);\
ECS_IMPORT_COMPONENT(handles, Position);\
ECS_IMPORT_COMPONENT(handles, Drawable);\
ECS_IMPORT_COMPONENT(handles, Input);\
ECS_IMPORT_TYPE(handles, Player);\
ECS_IMPORT_TYPE(handles, Builder);\
ECS_IMPORT_ENTITY(handles, EcsActor);\
ECS_IMPORT_ENTITY(handles, EcsPlayer);\
ECS_IMPORT_ENTITY(handles, EcsBuilder);\
ECS_IMPORT_ENTITY(handles, EcsDemoNPC);\
ECS_IMPORT_TYPE(handles, Movement);\
ECS_IMPORT_ENTITY(handles, Walking);\
ECS_IMPORT_ENTITY(handles, Flying);\
ECS_IMPORT_COMPONENT(handles, Velocity);\
ECS_IMPORT_ENTITY(handles, EcsClient);\
ECS_IMPORT_COMPONENT(handles, ClientInfo);\

void ComponentsImport(ecs_world_t *ecs);
