#define ZPL_NANO
#include "zpl.h"
#include "world/world.h"
#include "world/blocks.h"
#include "raylib.h"
#include "gen/texgen.h"
#include "world_view.h"
#include "perlin.h"

#define BLOCKS_COUNT (sizeof(blocks)/sizeof(block))

ZPL_TABLE(static, blocks__chunk_tbl, blocks__chunk_tbl_, RenderTexture2D);

static blocks__chunk_tbl baked_chunks;

static void chunks_unload_textures(uint64_t key, RenderTexture2D *value) {
    (void)key;
    UnloadRenderTexture(*value);
}

typedef struct {
    char *name;
    uint32_t flags;
    uint32_t kind;
    uint32_t biome;
    char symbol;
    float drag;
    float friction;
    float bounce;
    
    // NOTE(zaklaus): viewer data
    Texture2D img;
} block;

#include "blocks_list.c"

int32_t blocks_setup(void) {
    for (uint32_t i=0; i<BLOCKS_COUNT; i++) {
        block *b = &blocks[i];
        b->img = texgen_build_block(b->biome, b->kind);
    }
    
    blocks__chunk_tbl_init(&baked_chunks, zpl_heap());
    return 0;
}

void blocks_destroy(void) {
    for (uint32_t i=0; i<BLOCKS_COUNT; i++) {
        UnloadTexture(blocks[i].img);
    }
    
    blocks__chunk_tbl_map_mut(&baked_chunks, chunks_unload_textures);
    blocks__chunk_tbl_destroy(&baked_chunks);
}

uint8_t blocks_find(uint32_t biome, uint32_t kind) {
    for (uint32_t i=0; i<BLOCKS_COUNT; i++) {
        if (blocks[i].biome == biome && blocks[i].kind == kind)
            return i;
    }
    return BLOCK_INVALID;
}

char *blocks_get_name(uint8_t id) {
    return blocks[id].name;
}

char blocks_get_symbol(uint8_t id) {
    return blocks[id].symbol;
}

uint32_t blocks_get_flags(uint8_t id) {
    return blocks[id].flags;
}

uint32_t blocks_get_biome(uint8_t id) {
    return blocks[id].biome;
}

uint32_t blocks_get_kind(uint8_t id) {
    return blocks[id].kind;
}

float blocks_get_drag(uint8_t id) {
    return blocks[id].drag;
}

float blocks_get_friction(uint8_t id) {
    return blocks[id].friction;
}

float blocks_get_bounce(uint8_t id) {
    return blocks[id].bounce;
}

void *blocks_get_img(uint8_t id) {
    return (void*)&blocks[id].img;
}

void blocks_build_chunk_tex(uint64_t id, uint8_t *chunk_blocks, uint8_t *outer_chunk_blocks, void *raw_view) {
    world_view *view = (world_view*)raw_view;
    uint16_t blk_dims = WORLD_BLOCK_SIZE * 0.5f;
    uint16_t dims = blk_dims * view->chunk_size;
    RenderTexture2D canvas = LoadRenderTexture(dims, dims);
    BeginTextureMode(canvas);
    ClearBackground(WHITE);
    for (int y = 0; y < view->chunk_size; y += 1) {
        for (int x = 0; x < view->chunk_size; x += 1) {
#if 0
            Texture2D blk = blocks[chunk_blocks[(y*view->chunk_size)+x]].img;
            Rectangle src = {0, 0, WORLD_BLOCK_SIZE, WORLD_BLOCK_SIZE};
            Rectangle dst = {x*blk_dims, y*blk_dims, blk_dims, blk_dims};
            DrawTexturePro(blk, src, dst, (Vector2){0.0f,0.0f}, 0.0f, WHITE);
#else
            static float rots[] = { 0.0f, 90.0f, 180.f, 270.0f };
            float rot = rots[(int32_t)(perlin_fbm(view->seed, x, y, 1.2f, 3) * 4.0f) % 4];
            float half_block = blk_dims / 2.0f;
            Texture2D blk = blocks[chunk_blocks[(y*view->chunk_size)+x]].img;
            Rectangle src = {0, 0, WORLD_BLOCK_SIZE, WORLD_BLOCK_SIZE};
            Rectangle dst = {x*blk_dims + half_block, y*blk_dims + half_block, blk_dims, blk_dims};
            DrawTexturePro(blk, src, dst, (Vector2){half_block, half_block}, rot, WHITE);
            
            if (outer_chunk_blocks[(y*view->chunk_size)+x] != 0) {
                Texture2D blk2 = blocks[outer_chunk_blocks[(y*view->chunk_size)+x]].img;
                DrawTexturePro(blk2, src, dst, (Vector2){half_block, half_block}, rot, WHITE);
            }
#endif
        }
    }
    EndTextureMode();
    blocks__chunk_tbl_set(&baked_chunks, id, canvas);
}

void *blocks_get_chunk_tex(uint64_t id) {
    return blocks__chunk_tbl_get(&baked_chunks, id);
}

void blocks_remove_chunk_tex(uint64_t id) {
    RenderTexture2D *tex = blocks__chunk_tbl_get(&baked_chunks, id);
    if (!tex) return;
    UnloadRenderTexture(*tex);
    blocks__chunk_tbl_remove(&baked_chunks, id);
}