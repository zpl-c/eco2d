#pragma once
#include "system.h"

#define BLOCK_INVALID 0xF

typedef enum {
    BLOCK_FLAG_COLLISION = (1 << 1)
} block_flags;

#include "blocks_info.h"

int32_t blocks_setup(void);
void blocks_destroy(void);

uint8_t blocks_find(uint32_t biome, uint32_t kind);

char *blocks_get_name(uint8_t id);
char blocks_get_symbol(uint8_t id);
uint32_t blocks_get_flags(uint8_t id);
uint32_t blocks_get_biome(uint8_t id);
uint32_t blocks_get_kind(uint8_t id);

// NOTE(zaklaus): viewer-related functions
void *blocks_get_img(uint8_t id);

void blocks_build_chunk_tex(uint64_t id, uint8_t *blocks, size_t blocks_len, void *view);
void *blocks_get_chunk_tex(uint64_t id);
void blocks_remove_chunk_tex(uint64_t id);