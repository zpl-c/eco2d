#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

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
               uint8_t is_blocked;
               
               ecs_entity_t parent;
           });

ECS_STRUCT(ClientInfo, {
               uintptr_t peer;
               uint16_t view_id;                                             
           });

ECS_STRUCT(Health, {
               float hp;
               float max_hp;
               
               // NOTE(zaklaus): Intentionally global, to allow for creative use of damage combos
               float pain_time;
               float heal_time;
           });

ECS_STRUCT(Classify, {
               uint16_t id;
           });

ECS_STRUCT(Vehicle, {
               uint64_t seats[4];
               
               float force;
               float heading;
               float steer;
               float wheel_base;
           });

ECS_COMPONENT_EXTERN(Chunk);
ECS_COMPONENT_EXTERN(Position);
ECS_COMPONENT_EXTERN(Vector2D);
ECS_COMPONENT_EXTERN(Drawable);
ECS_COMPONENT_EXTERN(Input);
ECS_COMPONENT_EXTERN(Velocity);
ECS_COMPONENT_EXTERN(ClientInfo);
ECS_COMPONENT_EXTERN(Health);
ECS_COMPONENT_EXTERN(Classify);
ECS_COMPONENT_EXTERN(Vehicle);
ECS_TAG_EXTERN(EcsActor);
ECS_TAG_EXTERN(EcsDemoNPC);
ECS_TAG_EXTERN(EcsInVehicle);
ECS_TYPE_EXTERN(Player);
ECS_TYPE_EXTERN(Movement);
ECS_TYPE_EXTERN(Walking);
ECS_TYPE_EXTERN(Flying);
ECS_TYPE_EXTERN(EcsClient);

typedef struct {
    ECS_DECLARE_COMPONENT(Chunk);
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_COMPONENT(Vector2D);
    ECS_DECLARE_COMPONENT(Drawable);
    ECS_DECLARE_COMPONENT(Input);
    ECS_DECLARE_COMPONENT(Velocity);
    ECS_DECLARE_COMPONENT(ClientInfo);
    ECS_DECLARE_COMPONENT(Health);
    ECS_DECLARE_COMPONENT(Classify);
    ECS_DECLARE_COMPONENT(Vehicle);
    ECS_DECLARE_ENTITY(EcsActor);
    ECS_DECLARE_ENTITY(EcsDemoNPC);
    ECS_DECLARE_ENTITY(EcsInVehicle);
    ECS_DECLARE_TYPE(Player);
    ECS_DECLARE_TYPE(Builder);
    ECS_DECLARE_TYPE(Movement);
    ECS_DECLARE_ENTITY(Walking);
    ECS_DECLARE_ENTITY(Flying);
    ECS_DECLARE_ENTITY(EcsClient);
} Components;

#define ComponentsImportHandles(handles)\
ECS_IMPORT_COMPONENT(handles, Chunk);\
ECS_IMPORT_COMPONENT(handles, Vector2D);\
ECS_IMPORT_COMPONENT(handles, Position);\
ECS_IMPORT_COMPONENT(handles, Drawable);\
ECS_IMPORT_COMPONENT(handles, Input);\
ECS_IMPORT_COMPONENT(handles, Velocity);\
ECS_IMPORT_COMPONENT(handles, ClientInfo);\
ECS_IMPORT_COMPONENT(handles, Health);\
ECS_IMPORT_COMPONENT(handles, Classify);\
ECS_IMPORT_COMPONENT(handles, Vehicle);\
ECS_IMPORT_TYPE(handles, Player);\
ECS_IMPORT_TYPE(handles, Builder);\
ECS_IMPORT_TYPE(handles, Movement);\
ECS_IMPORT_ENTITY(handles, EcsActor);\
ECS_IMPORT_ENTITY(handles, EcsDemoNPC);\
ECS_IMPORT_ENTITY(handles, Walking);\
ECS_IMPORT_ENTITY(handles, Flying);\
ECS_IMPORT_ENTITY(handles, EcsClient);\
ECS_IMPORT_ENTITY(handles, EcsInVehicle);\

void ComponentsImport(ecs_world_t *ecs);
