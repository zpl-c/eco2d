#include "models/device.h"
#include "models/entity.h"
#include "world/entity_view.h"
#include "world/world.h"

#include "models/components.h"

uint64_t device_spawn(asset_id id) {
    ecs_entity_t e = entity_spawn(EKIND_DEVICE);

    Device *dev = ecs_get_mut(world_ecs(), e, Device);
    zpl_zero_item(dev);
    dev->asset = id;

    return (uint64_t)e;
}

void device_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
