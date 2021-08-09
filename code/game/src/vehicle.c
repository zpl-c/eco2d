#include "vehicle.h"
#include "entity.h"
#include "entity_view.h"
#include "world/world.h"

#include "modules/components.h"

uint64_t vehicle_spawn(void) {    
    ecs_entity_t e = entity_spawn(EKIND_VEHICLE);
    
    w_ecs_add(e, Vehicle);
    return (uint64_t)e;
}

void vehicle_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
