// NOTE(zaklaus): access to spawners
#include "storage.h"

static struct {
    asset_id id;
    uint64_t (*proc)();
} entity_spawnlist[] = {
    { .id = ASSET_CHEST, .proc = storage_spawn }
};

#define MAX_ENTITY_SPAWNDEFS ((sizeof(entity_spawnlist))/(sizeof(entity_spawnlist[0])))
