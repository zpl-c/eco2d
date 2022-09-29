#pragma once
#include "flecs.h"
#include "gen/assets.h"

#define ecs_get_mut_ex(world, entity, T) \
    (ECS_CAST(T*, world_component_cached(world, entity, ecs_id(T))))

#define ecs_get_if(world, entity, T) \
    (world_entity_valid(entity) ? ecs_get(world, entity, T) : NULL)

#define ecs_get_mut_if_ex(world, entity, component) \
    (ecs_get_if(world, entity, component) ? ecs_get_mut_ex(world, entity, component) : NULL)

#ifndef ecs_get_mut_if
#define ecs_get_mut_if(world, entity, component)\
    (ecs_get(world, entity, component) ? ecs_get_mut(world, entity, component) : NULL)
#endif

#define ITEMS_INVENTORY_SIZE 9
#define ITEMS_CONTAINER_SIZE 16

typedef struct {
    float x;
    float y;
} Vector2D;

typedef struct {
    uint32_t id;
    int16_t x;
    int16_t y;
    uint8_t is_dirty;
} Chunk;

typedef struct {
    uint16_t id;
} Drawable;

typedef Vector2D Position;
typedef Vector2D Velocity;

typedef struct {
    float x;
    float y;
    float mx;
    float my;
    float bx;
    float by;
    uint8_t use;
    uint8_t sprint;
    uint8_t ctrl;
    uint8_t pick;
    uint8_t is_blocked;
    ecs_entity_t pick_ent;
    ecs_entity_t sel_ent;

    // NOTE(zaklaus): inventory
    ecs_entity_t storage_ent;
    uint8_t storage_action;
    uint8_t selected_item;
    uint8_t storage_selected_item;
    uint8_t drop;
    uint8_t swap;
    uint8_t swap_storage;
    uint8_t swap_from;
    uint8_t swap_to;

    // NOTE(zaklaus): build mode
    uint8_t num_placements;
    float placements_x[20];
    float placements_y[20];
    uint8_t deletion_mode;
} Input;

typedef struct {
    uintptr_t peer;
    uint16_t view_id;
    uint8_t active;
} ClientInfo;

typedef struct {
    float hp;
    float max_hp;

    //NOTE(zaklaus): Intentionally global, to allow for creative use of damage combos
    float pain_time;
    float heal_time;
} Health;

typedef struct {
    uint16_t id;
} Classify;

typedef struct {
    uint64_t seats[4];

    float force;
    float heading;
    float steer;
    float wheel_base;

    float speed;
    float reverse_speed;
    uint8_t veh_kind;
} Vehicle;

typedef struct {
    ecs_entity_t veh;
} IsInVehicle;

typedef struct {
    uint16_t kind;
    uint32_t quantity;
    float merger_time;
    float durability; // 1.0 - 0.0 (0.0 = broken), we can only ever merge items of the same durability
} Item;

typedef struct {
    char _unused;
} BlockHarvest;

typedef struct {
    // TODO: we now hold a ref to an item, instead of representing an item slot,
    // so that we can let the item entity keep its own components and also handle merging ops on its own.
    ecs_entity_t items[ITEMS_INVENTORY_SIZE];
    float pickup_time;
} Inventory;

typedef struct {
    ecs_entity_t items[ITEMS_CONTAINER_SIZE];
} ItemContainer;

typedef struct {
    asset_id processed_item;
    float process_time;
    float energy_level;
} Producer;

typedef struct {
    asset_id kind;
    float energy_level;
} EnergySource;

typedef struct {
    asset_id producer;
    asset_id additional_ingredient; // optional - can specify additional item we need in the container to craft this item
    asset_id product;
} Ingredient;

typedef struct {
    uint16_t asset;

    // progress bar
    uint8_t progress_active;
    float progress_value;
} Device;

typedef struct {
    uint8_t w;
    uint8_t h;
    char plan[256];
} Blueprint;

typedef struct {
    double last_update;
    double tick_delay;
} StreamInfo;

typedef struct {char _unused;} DemoNPC;

extern ECS_COMPONENT_DECLARE(Vector2D);
extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Velocity);
extern ECS_COMPONENT_DECLARE(Chunk);
extern ECS_COMPONENT_DECLARE(Drawable);
extern ECS_COMPONENT_DECLARE(Input);
extern ECS_COMPONENT_DECLARE(ClientInfo);
extern ECS_COMPONENT_DECLARE(Health);
extern ECS_COMPONENT_DECLARE(Classify);
extern ECS_COMPONENT_DECLARE(Vehicle);
extern ECS_COMPONENT_DECLARE(IsInVehicle);
extern ECS_COMPONENT_DECLARE(Item);
extern ECS_COMPONENT_DECLARE(BlockHarvest);
extern ECS_COMPONENT_DECLARE(Inventory);
extern ECS_COMPONENT_DECLARE(ItemContainer);
extern ECS_COMPONENT_DECLARE(Producer);
extern ECS_COMPONENT_DECLARE(EnergySource);
extern ECS_COMPONENT_DECLARE(Ingredient);
extern ECS_COMPONENT_DECLARE(Device);
extern ECS_COMPONENT_DECLARE(Blueprint);
extern ECS_COMPONENT_DECLARE(DemoNPC);
extern ECS_COMPONENT_DECLARE(StreamInfo);

void ComponentsImport(ecs_world_t *ecs);
