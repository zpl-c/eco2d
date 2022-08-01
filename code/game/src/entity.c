#include "entity.h"
#include "entity_view.h"
#include "flecs/flecs.h"
#include "librg.h"
#include "world/world.h"

#include "modules/components.h"
#include "modules/systems.h"
#include "zpl.h"

uint64_t entity_spawn(uint16_t class_id) {
    ecs_entity_t e = ecs_new(world_ecs(), 0);

    ecs_set(world_ecs(), e, Classify, { .id = class_id });
    entity_wake(e);

    if (class_id != EKIND_SERVER) {
        ecs_set(world_ecs(), e, Velocity, {0});
        Position *pos = ecs_get_mut(world_ecs(), e, Position);
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

void entity_wake(uint64_t ent_id) {
    StreamInfo *si = ecs_get_mut(world_ecs(), ent_id, StreamInfo);
    si->tick_delay = 0.0f;
    si->last_update = 0.0f;
}

static ecs_query_t *ecs_streaminfo = NULL;

void entity_update_action_timers() {
    static double last_update_time = 0.0f;
    if (!ecs_streaminfo) {
        ecs_streaminfo = ecs_query_new(world_ecs(), "components.StreamInfo");
        last_update_time = zpl_time_rel();
    }

    ecs_iter_t it = ecs_query_iter(world_ecs(), ecs_streaminfo);

    while (ecs_query_next(&it)) {
        StreamInfo *si = ecs_field(&it, StreamInfo, 1);

        for (size_t i = 0; i < it.count; i++) {
            if (si[i].last_update < zpl_time_rel()) {
                si[i].last_update = zpl_time_rel() + si[i].tick_delay;
                si[i].tick_delay += (zpl_time_rel() - last_update_time) * 0.5f;
            }
        }
    }

    last_update_time = zpl_time_rel();
}

bool entity_can_stream(uint64_t ent_id) {
    StreamInfo *si = ecs_get_mut(world_ecs(), ent_id, StreamInfo);
    return (si->last_update < zpl_time_rel());
}
