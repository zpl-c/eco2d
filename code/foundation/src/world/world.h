#pragma once
#include "platform/system.h"
#include "librg.h"
#include "pkt/packet.h"
#include "flecs.h"

#include "world/blocks.h"

#define WORLD_ERROR_NONE                +0x0000
#define WORLD_ERROR_OUTOFMEM            -0x0001
#define WORLD_ERROR_INVALID_BLOCKS      -0x0002
#define WORLD_ERROR_INVALID_DIMENSIONS  -0x0003
#define WORLD_ERROR_INVALID_BUFFER      -0x0004
#define WORLD_ERROR_TRACKER_FAILED      -0x0005

#define WORLD_LAYERING 0
#define WORLD_TRACKER_LAYERS 3
#define WORLD_TRACKER_UPDATE_FAST_MS 0
#define WORLD_TRACKER_UPDATE_NORMAL_MS 0.05f
#define WORLD_TRACKER_UPDATE_SLOW_MS 0.1f
#define WORLD_TRACKER_UPDATE_MP_FAST_MS 0.05f
#define WORLD_TRACKER_UPDATE_MP_NORMAL_MS 0.15f
#define WORLD_TRACKER_UPDATE_MP_SLOW_MS 0.3f
#define WORLD_BLOCK_SIZE 64

#define WORLD_PKT_READER(name) int32_t name(void* data, uint32_t datalen, void *udata)
typedef WORLD_PKT_READER(world_pkt_reader_proc);

#define WORLD_PKT_WRITER(name) int32_t name(pkt_header *pkt, void *udata)
typedef WORLD_PKT_WRITER(world_pkt_writer_proc);

typedef struct {
	float minx, miny;
	float maxx, maxy;
} collision_island;

typedef struct {
    bool is_paused;
    block_id *data;
    block_id *outer_data;
    uint32_t seed;
    uint32_t size;
    uint16_t chunk_size;
    uint16_t chunk_amount;
    block_id **block_mapping;
    block_id **outer_block_mapping;
    uint16_t dim;
	uint8_t *islands_count;
	collision_island *islands;
    float tracker_update[3];
    uint8_t active_layer_id;
    ecs_world_t *ecs;
    ecs_world_t *ecs_stage;
    ecs_query_t *ecs_update;
    ecs_query_t *ecs_clientinfo;
	ecs_query_t *ecs_layeroverriden;
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

uint32_t world_buf(block_id const **ptr, uint32_t *width);
uint32_t world_seed(void);
ecs_world_t *world_ecs(void);
ecs_query_t *world_ecs_player(void);
ecs_query_t *world_ecs_clientinfo(void);
void world_set_stage(ecs_world_t *ecs);
librg_world *world_tracker(void);

// NOTE(zaklaus): World simulation time control
void world_pause(void);
void world_resume(void);
bool world_is_paused(void);
void world_step(float step_size);

uint16_t world_chunk_size(void);
uint16_t world_chunk_amount(void);
uint16_t world_dim(void);
ecs_entity_t world_chunk_mapping(librg_chunk id);
void world_rebuild_chunk_islands(librg_chunk chunk_id);
uint8_t world_chunk_collision_islands(librg_chunk id, collision_island *islands);

typedef struct {
    uint16_t id;
    block_id bid;
    ecs_entity_t chunk_e;
    int64_t chunk_id;
    float ox, oy;
    float aox, aoy;
    bool is_outer;
} world_block_lookup;

world_block_lookup world_block_from_realpos(float x, float y);
world_block_lookup world_block_from_index(int64_t id, uint16_t block_idx);
int64_t world_chunk_from_realpos(float x, float y);
int64_t world_chunk_from_entity(ecs_entity_t id);

// NOTE(zaklaus): This changes the inner chunk layer, it should only be used for terraforming!
void world_chunk_replace_worldgen_block(int64_t id, uint16_t block_idx, block_id bid);

// NOTE(zaklaus): Replaces a block unconditionally
void world_chunk_replace_block(int64_t id, uint16_t block_idx, block_id bid);

// NOTE(zaklaus): Places a block only if the outer layer's chunk slot is empty,
// it also allows us to remove a block from outer layer unconditionally
bool world_chunk_place_block(int64_t id, uint16_t block_idx, block_id bid);

// NOTE(zaklaus): Convenience method to replace block with air and drop item optionally
void world_chunk_destroy_block(float x, float y, bool drop_item);

block_id *world_chunk_get_blocks(int64_t id);
void world_chunk_mark_dirty(ecs_entity_t e);
bool world_chunk_is_dirty(ecs_entity_t e);

// NOTE(zaklaus): Uses locally persistent buffer !!
int64_t *world_chunk_fetch_entities(librg_chunk chunk_id, size_t *ents_len);
int64_t *world_chunk_fetch_entities_realpos(float x, float y, size_t *ents_len);
int64_t *world_chunk_query_entities(int64_t e, size_t *ents_len, int8_t radius);

bool world_entity_valid(ecs_entity_t e);
