#include "models/crafting.h"

#define R(id1,qty1)\
{\
.id = id1,\
.qty = qty1\
}

#define RECIPE(id,prod,qty,...)\
{\
.product = id,\
.product_qty = qty,\
.producer = prod,\
.reagents = (reagent[]){\
__VA_ARGS__\
}\
}

static recipe recipes[] = {
    RECIPE(ASSET_BELT, ASSET_FURNACE, 4, R(ASSET_FENCE, 8), R(ASSET_WOOD, 2), {0}),
};

#define MAX_RECIPES (sizeof(recipes)/sizeof(recipes[0]))

#undef R
#undef RECIPE