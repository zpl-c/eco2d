#include "player.h"
#include "world/entity_view.h"

#include "world/world.h"

#include "models/entity.h"
#include "models/components.h"

#define PLAYER_MAX_HP 100.0f

uint64_t player_spawn(char *name) {
    ecs_entity_t e = entity_spawn(EKIND_PLAYER);

    if (!name) {
        name = zpl_bprintf("player_%d", e);
    }

    ecs_set_name(world_ecs(), e, name);
    ecs_set(world_ecs(), e, ClientInfo, {0});
    ecs_set(world_ecs(), e, Input, {0});
    ecs_set(world_ecs(), e, Inventory, {0});
	ecs_set(world_ecs(), e, Health, {.hp = PLAYER_MAX_HP, .max_hp = PLAYER_MAX_HP});
	ecs_set(world_ecs(), e, HealthRegen, {.amt = 15.0f});
	ecs_set(world_ecs(), e, Velocity, { 0 });
	ecs_set(world_ecs(), e, PhysicsBody, { .kind = PHYS_AABB, .mass = INFINITE_MASS }); 

    librg_entity_owner_set(world_tracker(), e, (int64_t)e);

    return (uint64_t)e;
}

void player_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
