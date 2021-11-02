#pragma once
#include "system.h"
#include "assets.h"

typedef enum {
    BLOCK_FLAG_COLLISION = (1 << 1),
    BLOCK_FLAG_HAZARD = (1 << 2),
} block_flags;

int32_t blocks_setup(void);
void blocks_destroy(void);

uint8_t blocks_find(asset_id kind);

char *blocks_get_name(uint8_t id);
char blocks_get_symbol(uint8_t id);
uint32_t blocks_get_flags(uint8_t id);
float blocks_get_drag(uint8_t id);
float blocks_get_friction(uint8_t id);
float blocks_get_bounce(uint8_t id);

// NOTE(zaklaus): viewer-related functions
void *blocks_get_img(uint8_t id);

void blocks_build_chunk_tex(uint64_t id, uint8_t *blocks, void *view);
void *blocks_get_chunk_tex(uint64_t id);
void blocks_remove_chunk_tex(uint64_t id);