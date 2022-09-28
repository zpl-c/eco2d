#include "ents/items.h"
#include "world/entity_view.h"
#include "items_list_helpers.h"

static item_desc items[] = {
    { .kind = 0, .max_quantity = 0, },
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
    ITEM_ENT(ASSET_FURNACE, 32, ASSET_FURNACE),
};
