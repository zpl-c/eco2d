#pragma once
#include "platform/system.h"
#include "models/assets.h"

uint64_t device_spawn(asset_id id);
void device_despawn(uint64_t ent_id);
