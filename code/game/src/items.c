#include "items.h"
#include "entity.h"
#include "entity_view.h"
#include "world/world.h"

#include "modules/components.h"

uint64_t item_spawn(item_kind kind, uint32_t qty) {
    ecs_entity_t e = entity_spawn(EKIND_ITEM);
    
    ItemDrop *d = ecs_get_mut(world_ecs(), e, ItemDrop, NULL);
    *d = (ItemDrop){
        .kind = kind,
        .quantity = qty,
    };
    
    return (uint64_t)e;
}

void item_despawn(uint64_t id) {
    entity_despawn(id);
}

asset_id item_get_asset(item_kind kind) {
    switch (kind) {
        case IKIND_DEMO_ICEMAKER: return ASSET_DEMO_ICEMAKER;
    }
    
    ZPL_PANIC("unreachable code");
    return 0;
}