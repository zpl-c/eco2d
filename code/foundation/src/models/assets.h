#pragma once
#include "platform/system.h"

#include "lists/assets_ids.h"

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
