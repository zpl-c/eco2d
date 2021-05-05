#include "player.h"
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "librg.h"
#include "world.h"

#include "modules/general.h"
#include "modules/controllers.h"

uint64_t player_spawn(char *name) {
    ECS_IMPORT(world_ecs(), General);
    ECS_IMPORT(world_ecs(), Controllers);
    
    ecs_entity_t e = ecs_new(world_ecs(), 0);
    ecs_set(world_ecs(), e, EcsName, {.alloc_value = name });
    
    librg_entity_track(world_tracker(), e);
    librg_entity_owner_set(world_tracker(), e, (int64_t)e);
    librg_entity_radius_set(world_tracker(), e, 2); /* 2 chunk radius visibility */
    // librg_entity_chunk_set(world_tracker(), e, 1);
    
    return (uint64_t)e;
}

void player_despawn(uint64_t ent_id) {
    librg_entity_untrack(world_tracker(), ent_id);
    ecs_delete(world_ecs(), ent_id);
}