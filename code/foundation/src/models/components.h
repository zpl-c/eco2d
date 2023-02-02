#pragma once
#include "flecs.h"
#include "models/assets.h"

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

enum {
	PHYS_CIRCLE,
	PHYS_AABB,
};

#define INFINITE_MASS -1.0f

typedef struct {
	uint8_t kind;
	union {
		struct {
			float r;
		} circle;
 	};
	float mass;
} PhysicsBody;

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
    asset_id craft_item;
    
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
} Health;

typedef struct { char _unused; } Dead;

typedef struct {
	float amt; 
} HealthRegen;

typedef struct {
	uint8_t delay;
} HealDelay;

typedef struct {
	uint8_t _unused;
} HealthDecreased;

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

enum {
    PRODUCER_PUSH_PRODUCT,
    PRODUCER_PUSH_ANY,
    PRODUCER_PUSH_NONE,
};

enum {
    PRODUCER_CRAFT_WAITING,
    PRODUCER_CRAFT_BUSY,
    PRODUCER_CRAFT_ENQUEUED,
    PRODUCER_CRAFT_AUTO,
};

typedef struct {
    asset_id target_item;
    asset_id processed_item;
    uint32_t processed_item_qty;
    int32_t process_ticks;
	int32_t process_ticks_left;
    float energy_level;
    uint8_t pending_task;
    uint8_t push_filter;
} Producer;

typedef struct {
    uint32_t push_qty;
	uint8_t counter;
} ItemRouter;

typedef struct {
    asset_id kind;
    float energy_level;
} EnergySource;

typedef struct {
    uint16_t asset;
    
    // progress bar
    uint8_t progress_active;
    float progress_value;
} Device;

typedef struct {
    uint8_t w;
    uint8_t h;
    asset_id plan[256];
} Blueprint;

typedef struct {
    double last_update;
    double tick_delay;
} StreamInfo;

typedef struct {char _unused;} DemoNPC;

typedef struct {
	int16_t hunger_satisfied;
	int16_t mating_satisfied;
	int16_t life_remaining;
} Creature;

typedef struct { char _unused; } SeeksFood;
typedef struct { char _unused; } SeeksCompanion;

// survival comps
typedef struct { 
	uint8_t atk_delay;
} Mob;
typedef struct { 
	uint64_t plr; 
} MobHuntPlayer;

typedef struct { 
	char _unused; 
} MobMelee;

#define _COMPS\
	X(Vector2D)\
	X(Position)\
	X(Velocity)\
	X(PhysicsBody)\
	X(Chunk)\
	X(Drawable)\
	X(Input)\
	X(ClientInfo)\
	X(Health)\
	X(Dead)\
	X(HealthRegen)\
	X(HealDelay)\
	X(HealthDecreased)\
	X(Mob)\
	X(MobHuntPlayer)\
	X(MobMelee)\
	X(Classify)\
	X(Vehicle)\
	X(IsInVehicle)\
	X(Item)\
	X(BlockHarvest)\
	X(Inventory)\
	X(ItemContainer)\
	X(Producer)\
	X(EnergySource)\
	X(ItemRouter)\
	X(Device)\
	X(Blueprint)\
	X(DemoNPC)\
	X(Creature)\
	X(SeeksFood)\
	X(SeeksCompanion)\
	X(StreamInfo)

#define X(comp) extern ECS_COMPONENT_DECLARE(comp);
	_COMPS
#undef X

void ComponentsImport(ecs_world_t *ecs);
