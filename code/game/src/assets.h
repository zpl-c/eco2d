#pragma once
#include "system.h"

#define ASSET_INVALID 0xFF

typedef enum {
    // NOTE(zaklaus): Debug
    ASSET_EMPTY,
    ASSET_BUILDMODE_HIGHLIGHT,
    
    // NOTE(zaklaus): entities
    ASSET_PLAYER,
    ASSET_THING,
    
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
    
    ASSET_BELT,
    ASSET_BELT_LEFT,
    ASSET_BELT_RIGHT,
    ASSET_BELT_UP,
    ASSET_BELT_DOWN,
    
    MAX_ASSETS,
    FORCE_ASSET_UINT16 = UINT16_MAX
} asset_id;

typedef enum {
    AKIND_TEXTURE,
    AKIND_ANIM,
    AKIND_SOUND,
    
    FORCE_AKIND_UINT8 = UINT8_MAX
} asset_kind;

int32_t assets_setup(void);
int32_t assets_frame(void);
void assets_destroy(void);

uint16_t assets_find(asset_id id);

asset_kind assets_get_kind(uint16_t id);
void *assets_get_snd(uint16_t id);
void *assets_get_tex(uint16_t id);

// NOTE(zaklaus): client only
#define ASSET_SRC_RECT() ((Rectangle){0, 0, 64, 64})
#define ASSET_DST_RECT(x,y) ((Rectangle){x, y, 64, 64})
