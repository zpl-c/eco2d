#pragma once
#include "system.h"

#define WORLD_ERROR_NONE                +0x0000
#define WORLD_ERROR_OUTOFMEM            -0x0001
#define WORLD_ERROR_INVALID_BLOCKS      -0x0002
#define WORLD_ERROR_INVALID_DIMENSIONS  -0x0003
#define WORLD_ERROR_INVALID_BUFFER      -0x0004

int32_t world_init(int32_t seed, uint16_t chunk_width, uint16_t chunk_height, uint16_t chunk_amountx, uint16_t chunk_amounty);
int32_t world_destroy(void);

uint32_t world_buf(uint8_t const **ptr, uint32_t *width);
