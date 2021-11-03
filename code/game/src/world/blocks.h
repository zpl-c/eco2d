#pragma once
#include "system.h"
#include "assets.h"

typedef enum {
    BLOCK_FLAG_COLLISION = (1 << 1),
    BLOCK_FLAG_HAZARD = (1 << 2),
    BLOCK_FLAG_ESSENTIAL = (1 << 3),
} block_flags;

int32_t blocks_setup(void);
void blocks_destroy(void);

uint16_t blocks_find(asset_id kind);

asset_id blocks_get_asset(uint16_t id);
char blocks_get_symbol(uint16_t id);
uint32_t blocks_get_flags(uint16_t id);
float blocks_get_drag(uint16_t id);
float blocks_get_friction(uint16_t id);
float blocks_get_bounce(uint16_t id);
float blocks_get_velx(uint16_t id);
float blocks_get_vely(uint16_t id);

// NOTE(zaklaus): viewer-related functions
void *blocks_get_img(uint16_t id);

void blocks_build_chunk_tex(uint64_t id, uint16_t *blocks, void *view);
void *blocks_get_chunk_tex(uint64_t id);
void blocks_remove_chunk_tex(uint64_t id);