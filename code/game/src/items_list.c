#include "items.h"

#define ITEM_HOLD(asset, qty)\
{\
.kind = asset,\
.usage = UKIND_HOLD,\
.max_quantity = qty,\
}

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
    ITEM_SELF(ASSET_WOOD, 64),
    ITEM_HOLD(ASSET_TREE, 64),
    
    ITEM_SELF(ASSET_BELT_LEFT, 999),
    ITEM_SELF(ASSET_BELT_RIGHT, 999),
    ITEM_SELF(ASSET_BELT_UP, 999),
    ITEM_SELF(ASSET_BELT_DOWN, 999),
};
