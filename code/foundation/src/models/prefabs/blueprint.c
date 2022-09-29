#include "vehicle.h"

#include "world/entity_view.h"
#include "world/world.h"

#include "models/device.h"
#include "models/entity.h"
#include "models/items.h"
#include "models/components.h"

uint64_t blueprint_spawn(uint8_t w, uint8_t h, const char *plan) {
    ZPL_ASSERT((w*h) == zpl_strlen(plan));
    ZPL_ASSERT((w*h) < 256);
    ecs_entity_t e = device_spawn(ASSET_BLUEPRINT);

    Blueprint *blueprint = ecs_get_mut(world_ecs(), e, Blueprint);
    blueprint->w = w;
    blueprint->h = h;
    zpl_memcopy(blueprint->plan, plan, w*h);

    return (uint64_t)e;
}

uint64_t blueprint_spawn_udata(void* udata) {
    item_desc *it = (item_desc*)udata;
    return blueprint_spawn(it->blueprint.w, it->blueprint.h, it->blueprint.plan);
}

void blueprint_despawn(uint64_t id) {
    entity_despawn(id);
}
