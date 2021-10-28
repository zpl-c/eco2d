#include "entity.h"
#include "entity_view.h"
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "librg.h"
#include "world/world.h"

#include "modules/components.h"
#include "modules/systems.h"
#include "zpl.h"

uint64_t entity_spawn(uint16_t class_id) {
    ecs_entity_t e = ecs_new(world_ecs(), 0);
    
    ecs_set(world_ecs(), e, Classify, { .id = class_id });
    
    if (class_id != EKIND_SERVER) {
        ecs_set(world_ecs(), e, Velocity, {0});
        ecs_add(world_ecs(), e, Walking);
        Position *pos = ecs_get_mut(world_ecs(), e, Position, NULL);
#if 1
        pos->x=(float)(rand() % world_dim());
        pos->y=(float)(rand() % world_dim());
#else
        pos->x=350.0f;
        pos->y=88.0f;
#endif
        
        librg_entity_track(world_tracker(), e);
        librg_entity_chunk_set(world_tracker(), e, librg_chunk_from_realpos(world_tracker(), pos->x, pos->y, 0));
        librg_entity_owner_set(world_tracker(), e, (int64_t)e);
    }
    
    return (uint64_t)e;
}

void entity_batch_despawn(uint64_t *ids, size_t num_ids) {
    for (size_t i = 0; i < num_ids; i++ ) {
        librg_entity_untrack(world_tracker(), ids[i]);
        ecs_delete(world_ecs(), ids[i]);
    }
}

void entity_despawn(uint64_t ent_id) {
    librg_entity_untrack(world_tracker(), ent_id);
    ecs_delete(world_ecs(), ent_id);
}
