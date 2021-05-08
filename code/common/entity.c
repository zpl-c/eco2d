#include "entity.h"
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "librg.h"
#include "world.h"

#include "modules/general.h"
#include "modules/controllers.h"
#include "modules/net.h"
#include "modules/physics.h"
#include "zpl.h"

uint64_t entity_spawn(char *name) {
    ECS_IMPORT(world_ecs(), General);
    ECS_IMPORT(world_ecs(), Physics);
    
    ecs_entity_t e = ecs_new(world_ecs(), 0);
    
    if (!name) {
        name = zpl_bprintf("entity_%d", e);
    }
    
    ecs_set(world_ecs(), e, EcsName, {.alloc_value = name });
    ecs_set(world_ecs(), e, Velocity, {0});
    ecs_set(world_ecs(), e, Position, {0});
    ecs_add(world_ecs(), e, Walking);
    
    librg_entity_track(world_tracker(), e);
    librg_entity_chunk_set(world_tracker(), e, librg_chunk_from_realpos(world_tracker(), 0, 0, 0));
    
    return (uint64_t)e;
}

void entity_despawn(uint64_t ent_id) {
    librg_entity_untrack(world_tracker(), ent_id);
    ecs_delete(world_ecs(), ent_id);
}
