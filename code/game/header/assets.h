#pragma once
#include "system.h"

#define ASSET_INVALID 0xFF

typedef enum {
    ASSET_PLAYER,
    ASSET_THING,
    
    MAX_ASSETS,
    FORCE_ASSET_UINT16 = UINT16_MAX
} asset_id;

typedef enum {
    AKIND_TEXTURE,
    AKIND_SOUND,
    
    FORCE_AKIND_UINT8 = UINT8_MAX
} asset_kind;

int32_t assets_setup(void);
void assets_destroy(void);

uint16_t assets_find(asset_id id);

asset_kind assets_get_kind(uint16_t id);
void *assets_get_snd(uint16_t id);
void *assets_get_tex(uint16_t id);
