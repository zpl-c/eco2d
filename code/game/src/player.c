#include "player.h"
#include "entity.h"
#include "entity_view.h"
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "librg.h"
#include "world/world.h"

#include "modules/components.h"
#include "modules/systems.h"
#include "zpl.h"

#define PLAYER_MAX_HP 100.0f

uint64_t player_spawn(char *name) {    
    ecs_entity_t e = entity_spawn(EKIND_PLAYER);
    
    if (!name) {
        name = zpl_bprintf("player_%d", e);
    }
    
    ecs_set(world_ecs(), e, EcsName, {.alloc_value = name });
    ecs_add(world_ecs(), e, EcsClient);
    ecs_set(world_ecs(), e, ClientInfo, {0});
    ecs_set(world_ecs(), e, Input, {0});
    ecs_set(world_ecs(), e, Health, {.hp = PLAYER_MAX_HP, .max_hp = PLAYER_MAX_HP});
    ecs_add(world_ecs(), e, Player);
    
    librg_entity_owner_set(world_tracker(), e, (int64_t)e);
    
    return (uint64_t)e;
}

void player_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}

void player_freeze(uint64_t id, uint8_t state, uint8_t clear) {
}