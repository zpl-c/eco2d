#include "models/assets.h"

#define ASSET_ENTRY(asset, asset_kind)\
{\
.id = asset,\
.kind = asset_kind,\
}

#define ASSET_SND(asset) ASSET_ENTRY(asset, AKIND_SOUND)
#define ASSET_TEX(asset) ASSET_ENTRY(asset, AKIND_TEXTURE)
#define ASSET_ANI(asset) ASSET_ENTRY(asset, AKIND_ANIM)

static asset assets[] = {
    ASSET_TEX(ASSET_EMPTY),
    ASSET_TEX(ASSET_BLANK),
    ASSET_TEX(ASSET_BLOCK_FRAME),
    ASSET_TEX(ASSET_BUILDMODE_HIGHLIGHT),
    ASSET_TEX(ASSET_COAL),
    ASSET_TEX(ASSET_CHEST),
    ASSET_TEX(ASSET_FURNACE),
    ASSET_TEX(ASSET_BLUEPRINT),

    // NOTE(zaklaus): blocks
    ASSET_TEX(ASSET_FENCE),
    ASSET_TEX(ASSET_DEV),
    ASSET_TEX(ASSET_GROUND),
    ASSET_TEX(ASSET_DIRT),
    ASSET_ANI(ASSET_WATER),
    ASSET_TEX(ASSET_LAVA),
    ASSET_TEX(ASSET_WALL),
    ASSET_TEX(ASSET_HILL),
    ASSET_TEX(ASSET_HILL_SNOW),
    ASSET_TEX(ASSET_HOLE),
    ASSET_TEX(ASSET_WOOD),
    ASSET_TEX(ASSET_TEST_TALL),
    ASSET_TEX(ASSET_TREE),

    ASSET_TEX(ASSET_BELT),
    ASSET_TEX(ASSET_BELT_LEFT),
    ASSET_TEX(ASSET_BELT_RIGHT),
    ASSET_TEX(ASSET_BELT_UP),
    ASSET_TEX(ASSET_BELT_DOWN),
};
