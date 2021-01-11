#pragma once
#include "system.h"

#define BLOCKS_ERROR_NONE       +0x0000
#define BLOCKS_ERROR_OUTOFMEM   -0x0001
#define BLOCKS_ERROR_NOTFOUND   -0x0002
#define BLOCKS_ERROR_INVALID    -0x0003

typedef enum {
    BLOCK_FLAG_COLLISION = (1 << 0)
} block_flags;

int32_t blocks_init(void);
int32_t blocks_destroy(void);

// persisting buffer
char *blocks_get_name(uint8_t id);
uint32_t blocks_get_flags(uint8_t id);
