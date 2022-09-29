#pragma once
#include "platform/system.h"

enum {
    EVEH_CAR,
    EVEH_TRUCK,
    EVEH_FURNACEMOBILE,
};

uint64_t vehicle_spawn(uint8_t veh_kind);
void vehicle_despawn(uint64_t id);


