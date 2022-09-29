#pragma once

#define ASSET_INVALID 0xFF

typedef enum {
    // NOTE(zaklaus): Debug
    ASSET_EMPTY,
    ASSET_BLANK,
    ASSET_BLOCK_FRAME,
    ASSET_BUILDMODE_HIGHLIGHT,

    // NOTE(zaklaus): entities
    ASSET_PLAYER,
    ASSET_THING,
    ASSET_CHEST,
    ASSET_FURNACE,
    ASSET_BLUEPRINT,

    // NOTE(zaklaus): items
    ASSET_DEMO_ICEMAKER,

    // NOTE(zaklaus): blocks
    ASSET_FENCE,
    ASSET_DEV,
    ASSET_GROUND,
    ASSET_DIRT,
    ASSET_WATER,
    ASSET_LAVA,
    ASSET_WALL,
    ASSET_HILL,
    ASSET_HILL_SNOW,
    ASSET_HOLE,
    ASSET_WOOD,
    ASSET_TREE,
    ASSET_COAL,
    ASSET_IRON_ORE,
    ASSET_IRON_INGOT,

    ASSET_BELT,
    ASSET_BELT_LEFT,
    ASSET_BELT_RIGHT,
    ASSET_BELT_UP,
    ASSET_BELT_DOWN,

    MAX_ASSETS = 1024,
} asset_id;
