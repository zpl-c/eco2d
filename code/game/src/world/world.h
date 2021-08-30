#pragma once
#include "system.h"
#include "librg.h"
#include "packet.h"
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "modules/components.h"

#define WORLD_ERROR_NONE                +0x0000
#define WORLD_ERROR_OUTOFMEM            -0x0001
#define WORLD_ERROR_INVALID_BLOCKS      -0x0002
#define WORLD_ERROR_INVALID_DIMENSIONS  -0x0003
#define WORLD_ERROR_INVALID_BUFFER      -0x0004
#define WORLD_ERROR_TRACKER_FAILED      -0x0005

#define WORLD_LAYERING 0
#define WORLD_TRACKER_LAYERS 3
#define WORLD_TRACKER_UPDATE_FAST_MS 10
#define WORLD_TRACKER_UPDATE_NORMAL_MS 50
#define WORLD_TRACKER_UPDATE_SLOW_MS 100
#define WORLD_BLOCK_SIZE 64

#define WORLD_PKT_READER(name) int32_t name(void* data, uint32_t datalen, void *udata)
typedef WORLD_PKT_READER(world_pkt_reader_proc);

#define WORLD_PKT_WRITER(name) int32_t name(pkt_header *pkt, void *udata)
typedef WORLD_PKT_WRITER(world_pkt_writer_proc);

typedef struct {
    uint8_t *data;
    uint32_t seed;
    uint32_t size;
    uint16_t chunk_size;
    uint16_t chunk_amount;
    uint8_t **block_mapping;
    uint8_t **outer_block_mapping;
    uint16_t dim;
    uint64_t tracker_update[3];
    uint8_t active_layer_id;
    ecs_world_t *ecs;
    ecs_world_t *ecs_stage;
    ecs_query_t *ecs_update;
    ecs_entity_t *chunk_mapping;
    librg_world *tracker;
    world_pkt_reader_proc *reader_proc;
    world_pkt_writer_proc *writer_proc;
} world_data;

void world_setup_pkt_handlers(world_pkt_reader_proc *reader_proc, world_pkt_writer_proc *writer_proc);
int32_t world_init(int32_t seed, uint16_t chunk_size, uint16_t chunk_amount);
int32_t world_destroy(void);
int32_t world_update(void);

int32_t world_read(void* data, uint32_t datalen, void *udata);
int32_t world_write(pkt_header *pkt, void *udata);

uint32_t world_buf(uint8_t const **ptr, uint32_t *width);
ecs_world_t *world_ecs(void);
void world_set_stage(ecs_world_t *ecs);
librg_world *world_tracker(void);

uint16_t world_chunk_size(void);
uint16_t world_chunk_amount(void);
uint16_t world_dim(void);
ecs_entity_t world_chunk_mapping(librg_chunk id);

typedef struct {
    uint32_t id;
    uint8_t block_id;
    ecs_entity_t chunk_e;
    int64_t chunk_id;
    float ox, oy;
} world_block_lookup;

world_block_lookup world_block_from_realpos(float x, float y);
world_block_lookup world_block_from_index(int64_t id, uint16_t block_idx);
int64_t world_chunk_from_realpos(float x, float y);
int64_t world_chunk_from_entity(ecs_entity_t id);
void world_chunk_replace_block(int64_t id, uint16_t block_idx, uint8_t block_id);
bool world_chunk_place_block(int64_t id, uint16_t block_idx, uint8_t block_id);
uint8_t *world_chunk_get_blocks(int64_t id);
void world_chunk_mark_dirty(ecs_entity_t e);
uint8_t world_chunk_is_dirty(ecs_entity_t e);

// NOTE(zaklaus): Uses locally persistent buffer !!
int64_t *world_chunk_fetch_entities(librg_chunk chunk_id, size_t *ents_len);
int64_t *world_chunk_fetch_entities_realpos(float x, float y, size_t *ents_len);
int64_t *world_chunk_query_entities(int64_t e, size_t *ents_len, int8_t radius);

uint8_t world_entity_valid(ecs_entity_t e);