#include "models/device.h"
#include "models/assets.h"
#include "world/world.h"
#include "models/components.h"
#include "world/entity_view.h"
#include "world/world.h"
#include "models/device.h"
#include "models/entity.h"
#include "models/items.h"
#include "models/components.h"

//------------------------------------------------------------------------

uint64_t assembler_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_ASSEMBLER);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
    Producer *producer = ecs_get_mut(world_ecs(), e, Producer);
    *producer = (Producer){0};
    producer->energy_level = 69.0f;
    producer->pending_task = PRODUCER_CRAFT_AUTO;
    producer->push_filter = PRODUCER_PUSH_PRODUCT;
    producer->target_item = ASSET_INVALID;
    
    ecs_set(world_ecs(), e, ItemRouter, {.push_qty = 1, .counter = 0});
    return (uint64_t)e;
}

//------------------------------------------------------------------------

uint64_t blueprint_spawn(uint8_t w, uint8_t h, const asset_id *plan) {
    ZPL_ASSERT((w*h) < 256);
    ecs_entity_t e = device_spawn(ASSET_BLUEPRINT);

    Blueprint *blueprint = ecs_get_mut(world_ecs(), e, Blueprint);
    blueprint->w = w;
    blueprint->h = h;
    zpl_memcopy(blueprint->plan, plan, w*h*sizeof(asset_id));

    return (uint64_t)e;
}

uint64_t blueprint_spawn_udata(void* udata) {
    item_desc *it = (item_desc*)udata;
    return blueprint_spawn(it->blueprint.w, it->blueprint.h, it->blueprint.plan);
}

//------------------------------------------------------------------------

uint64_t craftbench_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_CRAFTBENCH);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
    Producer *producer = ecs_get_mut(world_ecs(), e, Producer);
    *producer = (Producer){0};
    producer->energy_level = 69.0f;
    producer->pending_task = PRODUCER_CRAFT_WAITING;
    producer->push_filter = PRODUCER_PUSH_NONE;
    return (uint64_t)e;
}

//------------------------------------------------------------------------

uint64_t creature_spawn(void) {
	ecs_entity_t e = entity_spawn(EKIND_DEMO_NPC);

	Creature *c = ecs_get_mut(world_ecs(), e, Creature);
	c->hunger_satisfied = 0;
	c->mating_satisfied = rand() % 1800;
	c->life_remaining = 500 + rand() % 5200;

	return (uint64_t)e;
}

//------------------------------------------------------------------------

uint64_t furnace_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_FURNACE);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
    Producer *producer = ecs_get_mut(world_ecs(), e, Producer);
    *producer = (Producer){0};
    producer->energy_level = 69.0f;
    producer->pending_task = PRODUCER_CRAFT_AUTO;
    producer->push_filter = PRODUCER_PUSH_ANY;
    
	ecs_set(world_ecs(), e, ItemRouter, {.push_qty = 1, .counter = 0});
    return (uint64_t)e;
}

//------------------------------------------------------------------------

uint64_t splitter_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_SPLITTER);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
	ecs_set(world_ecs(), e, ItemRouter, {.push_qty = 1, .counter = 0});
    return (uint64_t)e;
}

//------------------------------------------------------------------------

uint64_t storage_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_CHEST);

    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    return (uint64_t)e;
}

//------------------------------------------------------------------------

uint64_t mob_spawn(void) {
	ecs_entity_t e = entity_spawn(EKIND_MONSTER);

	ecs_add(world_ecs(), e, Mob);
	ecs_set(world_ecs(), e, Health, { 60, 60, 0 });
	ecs_set(world_ecs(), e, PhysicsBody, { .kind = PHYS_AABB, .mass = 1.0f });
    ecs_set(world_ecs(), e, Sprite, { .frame = 101 + (rand()%3) });

	return (uint64_t)e;
}
