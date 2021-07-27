#include "entity.h"
#include "entity_view.h"
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "librg.h"
#include "world/world.h"

#include "modules/components.h"
#include "modules/systems.h"
#include "zpl.h"

uint64_t entity_spawn(char *name, uint16_t class_id) {
    ECS_IMPORT(world_ecs(), Components);
    
    ecs_entity_t e = ecs_new(world_ecs(), 0);
    
    if (!name) {
        name = zpl_bprintf("entity_%d", e);
    }
    
    ecs_set(world_ecs(), e, EcsName, {.alloc_value = name });
    ecs_set(world_ecs(), e, Velocity, {0});
    ecs_set(world_ecs(), e, Classify, { .id = class_id });
    ecs_add(world_ecs(), e, Walking);
    Position *pos = ecs_get_mut(world_ecs(), e, Position, NULL);
#if 1
    pos->x=rand() % world_dim();
    pos->y=rand() % world_dim();
#else
    pos->x=350;
    pos->y=88;
#endif
    
    if (class_id != EKIND_SERVER) {
        librg_entity_track(world_tracker(), e);
        librg_entity_chunk_set(world_tracker(), e, librg_chunk_from_realpos(world_tracker(), pos->x, pos->y, 0));
    }
    
    return (uint64_t)e;
}

void entity_despawn(uint64_t ent_id) {
    librg_entity_untrack(world_tracker(), ent_id);
    ecs_delete(world_ecs(), ent_id);
}
