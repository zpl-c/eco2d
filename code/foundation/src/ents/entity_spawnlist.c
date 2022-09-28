// NOTE(zaklaus): access to spawners
#include "ents/storage.h"
#include "ents/furnace.h"

static struct {
    asset_id id;
    uint64_t (*proc)();
} entity_spawnlist[] = {
    { .id = ASSET_CHEST, .proc = storage_spawn },
    { .id = ASSET_FURNACE, .proc = furnace_spawn }
};

#define MAX_ENTITY_SPAWNDEFS ((sizeof(entity_spawnlist))/(sizeof(entity_spawnlist[0])))
