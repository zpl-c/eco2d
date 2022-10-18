#include "craftbench.h"
#include "models/device.h"
#include "world/world.h"

#include "models/entity.h"
#include "models/components.h"

uint64_t craftbench_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_CRAFTBENCH);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
    Producer *producer = ecs_get_mut(world_ecs(), e, Producer);
    *producer = (Producer){0};
    producer->energy_level = 69.0f;
    producer->pending_task = PRODUCER_CRAFT_WAITING;
    producer->push_filter = PRODUCER_PUSH_NONE;
    return (uint64_t)e;
}

void craftbench_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
