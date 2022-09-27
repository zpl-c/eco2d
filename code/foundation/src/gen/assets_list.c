#include "gen/assets.h"

static asset *assets = 0;

void assets_setup() {
    assets_register(ASSET_TEX(ASSET_EMPTY));
    assets_register(ASSET_TEX(ASSET_BLANK));
    assets_register(ASSET_TEX(ASSET_BUILDMODE_HIGHLIGHT));
    assets_register(ASSET_TEX(ASSET_DEMO_ICEMAKER));
    assets_register(ASSET_TEX(ASSET_CHEST));

    // NOTE: blocks
    assets_register(ASSET_TEX(ASSET_FENCE));
    assets_register(ASSET_TEX(ASSET_DEV));
    assets_register(ASSET_TEX(ASSET_GROUND));
    assets_register(ASSET_TEX(ASSET_DIRT));
    assets_register(ASSET_ANI(ASSET_WATER));
    assets_register(ASSET_TEX(ASSET_LAVA));
    assets_register(ASSET_TEX(ASSET_WALL));
    assets_register(ASSET_TEX(ASSET_HILL));
    assets_register(ASSET_TEX(ASSET_HILL_SNOW));
    assets_register(ASSET_TEX(ASSET_HOLE));
    assets_register(ASSET_TEX(ASSET_WOOD));
    assets_register(ASSET_TEX(ASSET_TREE));

    assets_register(ASSET_TEX(ASSET_BELT));
    assets_register(ASSET_TEX(ASSET_BELT_LEFT));
    assets_register(ASSET_TEX(ASSET_BELT_RIGHT));
    assets_register(ASSET_TEX(ASSET_BELT_UP));
    assets_register(ASSET_TEX(ASSET_BELT_DOWN));
};
