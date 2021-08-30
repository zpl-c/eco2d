#include "items.h"

static item_desc items[] = {
    {
        .kind = IKIND_DEMO_ICEMAKER,
        .usage = UKIND_PLACE,
        .asset = ASSET_DEMO_ICEMAKER,
        .max_quantity = 4,
        
        .place = {
            .biome = BLOCK_BIOME_DEV,
            .kind = BLOCK_KIND_WATER,
        }
    }
};