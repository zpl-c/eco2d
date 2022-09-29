#include "vehicle.h"

#include "world/entity_view.h"
#include "world/world.h"

#include "models/entity.h"
#include "models/components.h"


uint64_t vehicle_spawn(uint8_t veh_kind) {
    ecs_entity_t e = entity_spawn(EKIND_VEHICLE);

    Vehicle *veh = ecs_get_mut(world_ecs(), e, Vehicle);
    *veh = (Vehicle){
        .wheel_base = 50.0f,
        .speed = 50.0f,
        .reverse_speed = -20.0f,
        .force = 0.0f,
        .veh_kind = veh_kind,
    };

    switch (veh_kind) {
        case EVEH_CAR: {
            veh->wheel_base = 50.0f;
            veh->speed = 50.0f;
            veh->reverse_speed = -20.0f;
            veh->force = 0.0f;
        } break;
        case EVEH_TRUCK: {
            veh->wheel_base = 100.0f;
            veh->speed = 30.0f;
            veh->reverse_speed = -10.0f;
            veh->force = 0.0f;

            ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
            *storage = (ItemContainer){0};

            Device *dev = ecs_get_mut(world_ecs(), e, Device);
            dev->asset = ASSET_FURNACE;
        } break;
        case EVEH_FURNACEMOBILE: {
            veh->wheel_base = 100.0f;
            veh->speed = 30.0f;
            veh->reverse_speed = -10.0f;
            veh->force = 0.0f;

            ItemContainer *storage = ecs_get_mut(world_ecs(), e, ItemContainer);
            *storage = (ItemContainer){0};

            Device *dev = ecs_get_mut(world_ecs(), e, Device);
            dev->asset = ASSET_FURNACE;

            Producer *producer = ecs_get_mut(world_ecs(), e, Producer);
            *producer = (Producer){0};
            producer->energy_level = 69.0f;
        } break;
    }

    ecs_add(world_ecs(), e, BlockHarvest);
    return (uint64_t)e;
}

void vehicle_despawn(uint64_t ent_id) {
    entity_despawn(ent_id);
}
