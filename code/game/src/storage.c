#include "storage.h"
#include "device.h"
#include "entity.h"
#include "entity_view.h"
#include "world/world.h"

#include "modules/components.h"

uint64_t storage_spawn(void) {    
    ecs_entity_t e = device_spawn(ASSET_CHEST);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    return (uint64_t)e;
}

void storage_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
