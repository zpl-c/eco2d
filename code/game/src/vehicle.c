#include "vehicle.h"
#include "entity.h"
#include "entity_view.h"
#include "world/world.h"

#include "modules/components.h"

uint64_t vehicle_spawn(void) {    
    ecs_entity_t e = entity_spawn(EKIND_VEHICLE);
    
    Vehicle *veh = ecs_get_mut(world_ecs(), e, Vehicle, NULL);
    zpl_zero_item(veh);
    veh->wheel_base = 50.0f;
    veh->speed = 50.0f;
    veh->reverse_speed = -20.0f;
    veh->force = 0.0f;
    return (uint64_t)e;
}

void vehicle_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
