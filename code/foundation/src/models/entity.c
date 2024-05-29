#include "models/entity.h"
#include "world/entity_view.h"
#include "flecs.h"
#include "librg.h"
#include "world/world.h"

#include "models/components.h"
#include "systems/systems.h"
#include "zpl.h"

typedef struct {
    asset_id id;
    uint64_t (*proc)();
    uint64_t (*proc_udata)(void*);
} spawndef;

static spawndef *entity_spawnlist;

void entity_add_spawndef(uint16_t id, uint64_t (*proc)()) {
    spawndef def={0};
    def.id = id;
    def.proc = proc;
    zpl_array_append(entity_spawnlist, def);
}

void entity_add_spawndef_data(uint16_t id, uint64_t (*proc)(void*)) {
    spawndef def={0};
    def.id = id;
    def.proc_udata = proc;
    zpl_array_append(entity_spawnlist, def);
}

// NOTE(zaklaus): bring in entity spawnlist
// #include "lists/entity_spawnlist.c"
#include "models/prefabs/prefabs_list.c"
#define MAX_ENTITY_SPAWNDEFS ((size_t)zpl_array_count(entity_spawnlist))

void entity_default_spawnlist(void) {
    zpl_array_init(entity_spawnlist, zpl_heap());

    entity_add_spawndef(ASSET_CHEST, storage_spawn);
    entity_add_spawndef(ASSET_FURNACE, furnace_spawn);
    entity_add_spawndef(ASSET_CRAFTBENCH, craftbench_spawn);
    entity_add_spawndef(ASSET_SPLITTER, splitter_spawn);
    entity_add_spawndef(ASSET_ASSEMBLER, assembler_spawn);
	entity_add_spawndef(ASSET_CREATURE, creature_spawn);
	entity_add_spawndef(ASSET_MOB, mob_spawn);
    entity_add_spawndef_data(ASSET_BLUEPRINT, blueprint_spawn_udata);
}

uint64_t entity_spawn(uint16_t class_id) {
    ecs_entity_t e = ecs_new(world_ecs(), 0);

    ecs_set(world_ecs(), e, Classify, { .id = class_id });
    entity_wake(e);

    if (class_id != EKIND_SERVER) {
        librg_entity_track(world_tracker(), e);
        librg_entity_track(world_collision_grid(), e);
		ecs_set(world_ecs(), e, Velocity, { 0 });
        entity_set_position(e, (float)(rand() % world_dim()), (float)(rand() % world_dim()));

        librg_entity_owner_set(world_tracker(), e, (int64_t)e);
        librg_entity_owner_set(world_collision_grid(), e, (int64_t)e);
    }

    return (uint64_t)e;
}

uint64_t entity_spawn_id(uint16_t id){
    for (size_t i = 0; i < MAX_ENTITY_SPAWNDEFS; ++i){
        if (entity_spawnlist[i].id == id){
            ZPL_ASSERT(entity_spawnlist[i].proc);
            return entity_spawnlist[i].proc();
        }
    }
    return 0;
}

uint64_t entity_spawn_id_with_data(uint16_t id, void *udata){
    for (size_t i = 0; i < MAX_ENTITY_SPAWNDEFS; ++i){
        if (entity_spawnlist[i].id == id){
            ZPL_ASSERT(entity_spawnlist[i].proc_udata);
            return entity_spawnlist[i].proc_udata(udata);
        }
    }
    return 0;
}

bool entity_spawn_provided(uint16_t id) {
	for (size_t i = 0; i < MAX_ENTITY_SPAWNDEFS; ++i){
		if (entity_spawnlist[i].id == id){
			return true;
		}
	}
	return false;
}

void entity_batch_despawn(uint64_t *ids, size_t num_ids) {
    for (size_t i = 0; i < num_ids; i++ ) {
        librg_entity_untrack(world_collision_grid(), ids[i]);
        librg_entity_untrack(world_tracker(), ids[i]);
        ecs_delete(world_ecs(), ids[i]);
    }
}

void entity_despawn(uint64_t ent_id) {
    librg_entity_untrack(world_tracker(), ent_id);
    librg_entity_untrack(world_collision_grid(), ent_id);
    ecs_delete(world_ecs(), ent_id);
}

void entity_set_position(uint64_t ent_id, float x, float y) {
	ecs_set(world_ecs(), ent_id, Position, {x, y});
    Position *p = ecs_get_mut(world_ecs(), ent_id, Position);
    p->x = x;
    p->y = y;
    librg_entity_chunk_set(world_tracker(), ent_id, librg_chunk_from_realpos(world_tracker(), x, y, 0));
    librg_entity_chunk_set(world_collision_grid(), ent_id, librg_chunk_from_realpos(world_collision_grid(), x, y, 0));
    entity_wake(ent_id);
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
        last_update_time = get_cached_time();
    }

    ecs_iter_t it = ecs_query_iter(world_ecs(), ecs_streaminfo);

    while (ecs_query_next(&it)) {
        StreamInfo *si = ecs_field(&it, StreamInfo, 1);

        for (int32_t i = 0; i < it.count; i++) {
            if (si[i].last_update < get_cached_time()) {
                si[i].last_update = get_cached_time() + si[i].tick_delay;
                si[i].tick_delay += (get_cached_time() - last_update_time) * 0.5f;
            }
        }
    }

    last_update_time = get_cached_time();
}

bool entity_can_stream(uint64_t ent_id) {
    StreamInfo *si = ecs_get_mut(world_ecs(), ent_id, StreamInfo);
    return (si->last_update < get_cached_time());
}
