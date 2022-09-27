#include "ents/items.h"
#include "world/entity_view.h"
#include "items_list_helpers.h"

static item_desc *items = 0;

void item_setup() {
    item_register((item_desc){ .kind = 0, .max_quantity = 0, });
    item_register(ITEM_BLOCK(ASSET_DEMO_ICEMAKER, 64, ASSET_WATER));
    item_register(ITEM_SELF(ASSET_FENCE, 64));
    item_register(ITEM_SELF(ASSET_WOOD, 64));
    item_register(ITEM_HOLD(ASSET_TREE, 64));

    item_register(ITEM_SELF_DIR(ASSET_BELT, 999));
    item_register(ITEM_PROXY(ASSET_BELT_LEFT, ASSET_BELT));
    item_register(ITEM_PROXY(ASSET_BELT_RIGHT, ASSET_BELT));
    item_register(ITEM_PROXY(ASSET_BELT_UP, ASSET_BELT));
    item_register(ITEM_PROXY(ASSET_BELT_DOWN, ASSET_BELT));

    item_register(ITEM_ENT(ASSET_CHEST, 32, ASSET_CHEST));
}
