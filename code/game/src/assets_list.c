#include "assets.h"

#define ASSET_TEX(asset)\
{\
.id = asset,\
.kind = AKIND_TEXTURE,\
}

static asset assets[] = {
    ASSET_TEX(ASSET_EMPTY),
    ASSET_TEX(ASSET_DEMO_ICEMAKER),
    
    // NOTE(zaklaus): blocks
    ASSET_TEX(ASSET_FENCE),
    ASSET_TEX(ASSET_DEV),
    ASSET_TEX(ASSET_GROUND),
    ASSET_TEX(ASSET_DIRT),
    ASSET_TEX(ASSET_WATER),
    ASSET_TEX(ASSET_LAVA),
    ASSET_TEX(ASSET_WALL),
    ASSET_TEX(ASSET_HILL),
    ASSET_TEX(ASSET_HILL_SNOW),
    ASSET_TEX(ASSET_HOLE),
};
