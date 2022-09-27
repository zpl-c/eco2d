#pragma once
#include "platform/system.h"
#include "gen/assets.h"

#define BLOCK(a, f, s, ...)\
(block){\
.kind = a, .flags = f, .symbol = s, __VA_ARGS__\
}

typedef enum {
    BLOCK_FLAG_COLLISION = (1 << 1),
    BLOCK_FLAG_HAZARD = (1 << 2),
    BLOCK_FLAG_ESSENTIAL = (1 << 3),
    BLOCK_FLAG_DESTROY_ON_COLLISION = (1 << 4),
} block_flags;

typedef uint16_t block_id;

typedef struct {
    asset_id kind;
    uint32_t flags;
    char symbol;
    float drag;
    float friction;
    float bounce;

    float velx;
    float vely;

    // NOTE(zaklaus): viewer data
    block_id slot;
} block;

void blocks_setup(void);
void blocks_register(block desc);
void blocks_cleanup(void);

// resources
int32_t blocks_resources_setup(void);
void blocks_resources_destroy(void);

block_id blocks_find(asset_id kind);

asset_id blocks_get_asset(block_id id);
char blocks_get_symbol(block_id id);
uint32_t blocks_get_flags(block_id id);
float blocks_get_drag(block_id id);
float blocks_get_friction(block_id id);
float blocks_get_bounce(block_id id);
float blocks_get_velx(block_id id);
float blocks_get_vely(block_id id);

// NOTE(zaklaus): viewer-related functions
void *blocks_get_img(block_id id);

void blocks_build_chunk_tex(uint64_t id, block_id *blocks, void *view);
void *blocks_get_chunk_tex(uint64_t id);
void blocks_remove_chunk_tex(uint64_t id);
