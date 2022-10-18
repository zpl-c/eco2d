#include "models/items.h"
#include "world/entity_view.h"
#include "items_list_helpers.h"

static item_desc items[] = {
    { .kind = 0, .max_quantity = 0, },
    ITEM_HOLD(ASSET_FENCE, 64),
    ITEM_ENERGY(ASSET_COAL, ASSET_FURNACE, 64, 15.0f),
    ITEM_SELF(ASSET_WOOD, 64),
    ITEM_SELF(ASSET_TREE, 64),
    ITEM_SELF(ASSET_TEST_TALL, 64),
    
    // ITEM_BLUEPRINT(ASSET_BLUEPRINT, 1, 4, 4, "]]]]]CF]   ]]]]]"),
    ITEM_BLUEPRINT_PROXY(ASSET_BLUEPRINT_DEMO_HOUSE, ASSET_BLUEPRINT, 1, 4, 4, PROT({ ASSET_WOOD,ASSET_WOOD,ASSET_WOOD,ASSET_WOOD,
                                                                                        ASSET_WOOD,ASSET_FURNACE,ASSET_CHEST,ASSET_WOOD,
                                                                                        ASSET_WOOD,ASSET_EMPTY,ASSET_EMPTY,ASSET_WOOD,
                                                                                        ASSET_WOOD,ASSET_WOOD,ASSET_EMPTY,ASSET_WOOD})),
    
    ITEM_SELF_DIR(ASSET_BELT, 999),
    ITEM_PROXY(ASSET_BELT_LEFT, ASSET_BELT),
    ITEM_PROXY(ASSET_BELT_RIGHT, ASSET_BELT),
    ITEM_PROXY(ASSET_BELT_UP, ASSET_BELT),
    ITEM_PROXY(ASSET_BELT_DOWN, ASSET_BELT),
    
    ITEM_ENT(ASSET_CHEST, 32, ASSET_CHEST),
    ITEM_ENT(ASSET_CRAFTBENCH, 32, ASSET_CRAFTBENCH),
    ITEM_ENT(ASSET_FURNACE, 32, ASSET_FURNACE),
    ITEM_ENT(ASSET_SPLITTER, 32, ASSET_SPLITTER),
    
    ITEM_HOLD(ASSET_IRON_ORE, 64),
    ITEM_HOLD(ASSET_IRON_INGOT, 64),
    ITEM_HOLD(ASSET_IRON_PLATES, 64),
    ITEM_HOLD(ASSET_SCREWS, 64),
    ITEM_HOLD(ASSET_LOG, 64),
    ITEM_HOLD(ASSET_PLANK, 64),
};
