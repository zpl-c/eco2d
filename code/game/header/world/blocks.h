#pragma once
#include "system.h"

#define BLOCK_INVALID 0xF

typedef enum {
    BLOCK_FLAG_COLLISION = (1 << 0)
} block_flags;

#include "blocks_info.h"

uint8_t blocks_find(uint32_t biome, uint32_t kind);

char *blocks_get_name(uint8_t id);
char blocks_get_symbol(uint8_t id);
uint32_t blocks_get_flags(uint8_t id);
uint32_t blocks_get_biome(uint8_t id);
uint32_t blocks_get_kind(uint8_t id);
