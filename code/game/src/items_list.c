#include "items.h"
#include "entity_view.h"

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

#define ITEM_BLOCK_DIR(asset, qty, build_asset)\
{\
.kind = asset,\
.usage = UKIND_PLACE,\
.max_quantity = qty,\
.place = {\
.kind = build_asset,\
.directional = true,\
}\
}

#define ITEM_PROXY(asset, proxy_id)\
{\
.kind = asset,\
.usage = UKIND_PROXY,\
.proxy = {\
.id = proxy_id,\
}\
}

#define ITEM_ENT(asset, qty, eid)\
{\
.kind = asset,\
.usage = UKIND_PLACE_ITEM,\
.max_quantity = qty,\
.place_item = {\
.id = eid\
}\
}

#define ITEM_SELF(asset, qty) ITEM_BLOCK(asset, qty, asset)
#define ITEM_SELF_DIR(asset, qty) ITEM_BLOCK_DIR(asset, qty, asset)

static item_desc items[] = {
    {
        .kind = 0,
        .max_quantity = 0,
    },
    ITEM_BLOCK(ASSET_DEMO_ICEMAKER, 64, ASSET_WATER),
    ITEM_SELF(ASSET_FENCE, 64),
    ITEM_SELF(ASSET_WOOD, 64),
    ITEM_HOLD(ASSET_TREE, 64),
    
    ITEM_SELF_DIR(ASSET_BELT, 999),
    ITEM_PROXY(ASSET_BELT_LEFT, ASSET_BELT),
    ITEM_PROXY(ASSET_BELT_RIGHT, ASSET_BELT),
    ITEM_PROXY(ASSET_BELT_UP, ASSET_BELT),
    ITEM_PROXY(ASSET_BELT_DOWN, ASSET_BELT),
    
    ITEM_ENT(ASSET_CHEST, 32, ASSET_CHEST),
};
