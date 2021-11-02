#include "items.h"

#define ITEM_BLOCK(asset, qty, build_asset)\
{\
.kind = asset,\
.usage = UKIND_PLACE,\
.max_quantity = qty,\
.place = {\
.kind = build_asset,\
}\
}

#define ITEM_SELF(asset, qty) ITEM_BLOCK(asset, qty, asset)

static item_desc items[] = {
    {
        .kind = 0,
        .max_quantity = 0,
    },
    ITEM_BLOCK(ASSET_DEMO_ICEMAKER, 64, ASSET_WATER),
    ITEM_SELF(ASSET_FENCE, 64),
};
