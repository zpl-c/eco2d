#include "items.h"

static item_desc items[] = {
    {
        .kind = 0,
        .max_quantity = 0,
    },
    {
        .kind = ASSET_DEMO_ICEMAKER,
        .usage = UKIND_PLACE,
        .max_quantity = 64,
        
        .place = {
            .biome = BLOCK_BIOME_DEV,
            .kind = BLOCK_KIND_WATER,
        }
    }
};