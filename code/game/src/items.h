#pragma once
#include "system.h"
#include "assets.h"

typedef enum {
    IKIND_DEMO_ICEMAKER,
} item_kind;

uint64_t item_spawn(item_kind kind, uint32_t qty);
void item_despawn(uint64_t id);

// NOTE(zaklaus): client

asset_id item_get_asset(item_kind kind);