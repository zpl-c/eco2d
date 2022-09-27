#pragma once
#include "platform/system.h"

#define ASSET_INVALID 0xFF

#define ASSET_ENTRY(asset, asset_kind)\
(asset_desc){\
.id = asset,\
.kind = asset_kind,\
}

#define ASSET_SND(asset) ASSET_ENTRY(asset, AKIND_SOUND)
#define ASSET_TEX(asset) ASSET_ENTRY(asset, AKIND_TEXTURE)
#define ASSET_ANI(asset) ASSET_ENTRY(asset, AKIND_ANIM)

typedef enum {
    // NOTE(zaklaus): Debug
    ASSET_EMPTY,
    ASSET_BLANK,
    ASSET_BUILDMODE_HIGHLIGHT,

    // NOTE(zaklaus): entities
    ASSET_PLAYER,
    ASSET_THING,
    ASSET_CHEST,

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

    ASSET_NEXT_FREE,

    MAX_ASSETS = 1024,
} asset_id;

typedef enum {
    AKIND_TEXTURE,
    AKIND_ANIM,
    AKIND_SOUND,

    FORCE_AKIND_UINT8 = UINT8_MAX
} asset_kind;

typedef struct {
    asset_id id;
    asset_kind kind;
} asset_desc;

void assets_setup(void);
void assets_cleanup(void);
void assets_register(asset_desc desc);

// resources
int32_t assets_resources_setup(void);
void assets_resources_destroy(void);
int32_t assets_resources_frame(void);

uint16_t assets_find(asset_id id);

asset_kind assets_get_kind(uint16_t id);
void *assets_get_snd(uint16_t id);
void *assets_get_tex(uint16_t id);

// NOTE(zaklaus): client only
#define ASSET_SRC_RECT() ((Rectangle){0, 0, 64, 64})
#define ASSET_DST_RECT(x,y) ((Rectangle){x, y, 64, 64})
