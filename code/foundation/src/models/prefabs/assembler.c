#include "assembler.h"
#include "models/device.h"
#include "world/world.h"

#include "models/entity.h"
#include "models/components.h"

uint64_t assembler_spawn(void) {
    ecs_entity_t e = device_spawn(ASSET_ASSEMBLER);
    
    ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
    *storage = (ItemContainer){0};
    
    Producer *producer = ecs_get_mut(world_ecs(), e, Producer);
    *producer = (Producer){0};
    producer->energy_level = 69.0f;
    producer->pending_task = PRODUCER_CRAFT_AUTO;
    producer->push_filter = PRODUCER_PUSH_PRODUCT;
    producer->target_item = ASSET_INVALID;
    
    ecs_set(world_ecs(), e, ItemRouter, {.push_qty = 1, .counter = 0});
    return (uint64_t)e;
}

void assembler_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
