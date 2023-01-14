#include "models/crafting.h"

#define R(id1,qty1)\
{\
.id = id1,\
.qty = qty1\
}

#define RECIPE(id,prod,qty,ticks,...)\
{\
.product = id,\
.product_qty = qty,\
.process_ticks = ticks,\
.producer = prod,\
.reagents = (reagent[]){\
__VA_ARGS__\
}\
}

static recipe recipes[] = {
    // NOTE(zaklaus): Belt
    RECIPE(ASSET_IRON_PLATES, ASSET_FURNACE, 4, 20, R(ASSET_IRON_ORE, 1), {0}),
    RECIPE(ASSET_SCREWS, ASSET_CRAFTBENCH, 8, 40, R(ASSET_IRON_PLATES, 1), {0}),
    RECIPE(ASSET_BELT, ASSET_ASSEMBLER, 1,120, R(ASSET_FENCE, 1), R(ASSET_SCREWS, 4), R(ASSET_IRON_PLATES, 2), {0}),
};

#define MAX_RECIPES (sizeof(recipes)/sizeof(recipes[0]))

#undef R
#undef RECIPE
