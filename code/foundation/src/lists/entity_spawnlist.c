// NOTE(zaklaus): access to spawners
#include "models/prefabs/storage.h"
#include "models/prefabs/furnace.h"
#include "models/prefabs/blueprint.h"

static struct {
    asset_id id;
    uint64_t (*proc)();
    uint64_t (*proc_udata)(void*);
} entity_spawnlist[] = {
    { .id = ASSET_CHEST, .proc = storage_spawn },
    { .id = ASSET_FURNACE, .proc = furnace_spawn },
    { .id = ASSET_BLUEPRINT, .proc_udata = blueprint_spawn_udata },
};

#define MAX_ENTITY_SPAWNDEFS ((sizeof(entity_spawnlist))/(sizeof(entity_spawnlist[0])))
