#include "vehicle.h"

#include "world/entity_view.h"
#include "world/world.h"

#include "models/entity.h"
#include "models/components.h"

uint64_t vehicle_spawn(void) {    
    ecs_entity_t e = entity_spawn(EKIND_VEHICLE);
    
    Vehicle *veh = ecs_get_mut(world_ecs(), e, Vehicle);
    *veh = (Vehicle){
        .wheel_base = 50.0f,
        .speed = 50.0f,
        .reverse_speed = -20.0f,
        .force = 0.0f,
    };
    return (uint64_t)e;
}

void vehicle_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
