#pragma once
#include "system.h"
#include "librg.h"
#include "packet.h"
#include "flecs/flecs.h"

#define WORLD_ERROR_NONE                +0x0000
#define WORLD_ERROR_OUTOFMEM            -0x0001
#define WORLD_ERROR_INVALID_BLOCKS      -0x0002
#define WORLD_ERROR_INVALID_DIMENSIONS  -0x0003
#define WORLD_ERROR_INVALID_BUFFER      -0x0004
#define WORLD_ERROR_TRACKER_FAILED      -0x0005

#define WORLD_LAYERING 0
#define WORLD_TRACKER_LAYERS 3
#define WORLD_TRACKER_UPDATE_FAST_MS 10
#define WORLD_TRACKER_UPDATE_NORMAL_MS 100
#define WORLD_TRACKER_UPDATE_SLOW_MS 400

#define WORLD_PKT_READER(name) int32_t name(void* data, uint32_t datalen, void *udata)
typedef WORLD_PKT_READER(world_pkt_reader_proc);

#define WORLD_PKT_WRITER(name) int32_t name(pkt_header *pkt, void *udata)
typedef WORLD_PKT_WRITER(world_pkt_writer_proc);

void world_setup_pkt_handlers(world_pkt_reader_proc *reader_proc, world_pkt_writer_proc *writer_proc);
int32_t world_init(int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t chunk_amount);
int32_t world_destroy(void);
int32_t world_update(void);
int32_t world_read(void* data, uint32_t datalen, void *udata);
int32_t world_write(pkt_header *pkt, void *udata);

uint32_t world_buf(uint8_t const **ptr, uint32_t *width);
ecs_world_t * world_ecs(void);
librg_world * world_tracker(void);

uint16_t world_block_size(void);
uint16_t world_chunk_size(void);
uint16_t world_chunk_amount(void);
uint16_t world_dim(void);
