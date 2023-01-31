// NOTE(zaklaus): access to spawners
#include "models/prefabs/prefabs_list.c"
static struct {
    asset_id id;
    uint64_t (*proc)();
    uint64_t (*proc_udata)(void*);
} entity_spawnlist[] = {
    { .id = ASSET_CHEST, .proc = storage_spawn },
    { .id = ASSET_FURNACE, .proc = furnace_spawn },
    { .id = ASSET_CRAFTBENCH, .proc = craftbench_spawn },
    { .id = ASSET_SPLITTER, .proc = splitter_spawn },
    { .id = ASSET_ASSEMBLER, .proc = assembler_spawn },
    { .id = ASSET_BLUEPRINT, .proc_udata = blueprint_spawn_udata },
	{ .id = ASSET_CREATURE, .proc = creature_spawn },
	{ .id = ASSET_MOB, .proc = mob_spawn },
};

#define MAX_ENTITY_SPAWNDEFS ((sizeof(entity_spawnlist))/(sizeof(entity_spawnlist[0])))
