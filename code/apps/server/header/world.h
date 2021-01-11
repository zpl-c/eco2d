#pragma once
#include "system.h"

#define WORLD_ERROR_NONE                +0x0000
#define WORLD_ERROR_OUTOFMEM            -0x0001
#define WORLD_ERROR_INVALID_BLOCKS      -0x0002
#define WORLD_ERROR_INVALID_DIMENSIONS  -0x0003
#define WORLD_ERROR_INVALID_BUFFER      -0x0004

int32_t world_init(int32_t seed, uint32_t width, uint32_t height);
int32_t world_destroy(void);

uint32_t world_buf(uint8_t const **ptr, uint32_t *width);