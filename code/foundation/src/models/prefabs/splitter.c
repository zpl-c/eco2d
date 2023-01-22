#include "splitter.h"
#include "models/device.h"
#include "world/world.h"

#include "models/entity.h"
#include "models/components.h"

uint64_t splitter_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_SPLITTER);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
	ecs_set(world_ecs(), e, ItemRouter, {.push_qty = 1, .counter = 0});
    return (uint64_t)e;
}

void splitter_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
