#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

//NOTE(zaklaus): custom macro to define meta components outside the current scope

#ifndef ECS_META_DEFINE
#define ECS_META_DEFINE(world, T)\
ECS_COMPONENT_DEFINE(world, T);\
ecs_new_meta(world, ecs_entity(T), &__##T##__);
#endif

#ifndef ecs_get_mut_if
#define ecs_get_mut_if(world, entity, component)\
(ecs_get(world, entity, component) ? ecs_get_mut(world, entity, component, NULL) : NULL)
#endif

#define ITEMS_INVENTORY_SIZE 9

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
               float mx;
               float my;
               uint8_t use;
               uint8_t sprint;
               uint8_t ctrl;
               uint8_t is_blocked;
               
               // NOTE(zaklaus): inventory
               uint8_t selected_item;
               uint8_t drop;
               uint8_t swap;
               uint8_t swap_from;
               uint8_t swap_to;
               
               // NOTE(zaklaus): build mode
               uint8_t num_placements;
               float placements_x[20];
               float placements_y[20];
           });

ECS_STRUCT(ClientInfo, {
               uintptr_t peer;
               uint16_t view_id;
               uint8_t active;
           });

ECS_STRUCT(Health, {
               float hp;
               float max_hp;
               
               //NOTE(zaklaus): Intentionally global, to allow for creative use of damage combos
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
               
               float speed;
               float reverse_speed;
           });

typedef struct {
    ecs_entity_t veh;
} IsInVehicle;

typedef struct {
    uint16_t kind;
    uint32_t quantity;
    float merger_time;
} ItemDrop;

typedef struct {
    ItemDrop items[ITEMS_INVENTORY_SIZE];
    float pickup_time;
} Inventory;

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
ECS_COMPONENT_EXTERN(IsInVehicle);
ECS_COMPONENT_EXTERN(ItemDrop);
ECS_COMPONENT_EXTERN(Inventory);
ECS_TAG_EXTERN(EcsActor);
ECS_TAG_EXTERN(EcsDemoNPC);
ECS_TYPE_EXTERN(Player);
ECS_TYPE_EXTERN(Movement);
ECS_TYPE_EXTERN(Walking);
ECS_TYPE_EXTERN(Flying);
ECS_TYPE_EXTERN(EcsClient);
// NOTE(zaklaus): @1 EXTERN

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
    ECS_DECLARE_COMPONENT(IsInVehicle);
    ECS_DECLARE_COMPONENT(ItemDrop);
    ECS_DECLARE_COMPONENT(Inventory);
    ECS_DECLARE_ENTITY(EcsActor);
    ECS_DECLARE_ENTITY(EcsDemoNPC);
    ECS_DECLARE_TYPE(Player);
    ECS_DECLARE_TYPE(Builder);
    ECS_DECLARE_TYPE(Movement);
    ECS_DECLARE_ENTITY(Walking);
    ECS_DECLARE_ENTITY(Flying);
    // NOTE(zaklaus): @2 DECLARE
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
ECS_IMPORT_COMPONENT(handles, IsInVehicle);\
ECS_IMPORT_COMPONENT(handles, ItemDrop);\
ECS_IMPORT_COMPONENT(handles, Inventory);\
ECS_IMPORT_TYPE(handles, Player);\
ECS_IMPORT_TYPE(handles, Builder);\
ECS_IMPORT_TYPE(handles, Movement);\
ECS_IMPORT_ENTITY(handles, EcsActor);\
ECS_IMPORT_ENTITY(handles, EcsDemoNPC);\
ECS_IMPORT_ENTITY(handles, Walking);\
ECS_IMPORT_ENTITY(handles, Flying);\
// NOTE(zaklaus): @3 IMPORT

void ComponentsImport(ecs_world_t *ecs);
