#include "furnace.h"
#include "models/device.h"
#include "world/world.h"

#include "models/entity.h"
#include "models/components.h"

uint64_t furnace_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_FURNACE);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
    Producer *producer = ecs_get_mut(world_ecs(), e, Producer);
    *producer = (Producer){0};
    producer->energy_level = 69.0f;
    
    ecs_add(world_ecs(), e, ItemRouter);
    return (uint64_t)e;
}

void furnace_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
