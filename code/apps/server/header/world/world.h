#pragma once
#include "system.h"
#include "librg.h"
#include "flecs/flecs.h"

#define WORLD_ERROR_NONE                +0x0000
#define WORLD_ERROR_OUTOFMEM            -0x0001
#define WORLD_ERROR_INVALID_BLOCKS      -0x0002
#define WORLD_ERROR_INVALID_DIMENSIONS  -0x0003
#define WORLD_ERROR_INVALID_BUFFER      -0x0004
#define WORLD_ERROR_TRACKER_FAILED      -0x0005

int32_t world_init(int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size);
int32_t world_destroy(void);
int32_t world_update(void);

uint32_t world_buf(uint8_t const **ptr, uint32_t *width);
ecs_world_t * world_ecs(void);
librg_world * world_tracker(void);

uint16_t world_chunk_size(void);
uint16_t world_chunk_amount(void);