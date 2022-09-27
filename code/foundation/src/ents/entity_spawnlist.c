// NOTE(zaklaus): access to spawners
#include "ents/storage.h"

static spawndef *entity_spawnlist = 0;

void entity_spawndef_setup(void) {
    entity_spawndef_register((spawndef){ .id = ASSET_CHEST, .proc = storage_spawn });
}
