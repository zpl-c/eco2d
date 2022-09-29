#define ZPL_NANO
#include "zpl.h"
#include "world/world.h"
#include "world/blocks.h"
#include "raylib.h"
#include "gen/texgen.h"
#include "world/world_view.h"
#include "perlin.h"

#define BLOCKS_COUNT (sizeof(blocks)/sizeof(block))
#define WORLD_TEXTURE_BLOCK_SCALE 0.5f

ZPL_TABLE(static, blocks__chunk_tbl, blocks__chunk_tbl_, RenderTexture2D);

static blocks__chunk_tbl baked_chunks;

static void chunks_unload_textures(uint64_t key, RenderTexture2D *value) {
    (void)key;
    UnloadRenderTexture(*value);
}

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

#include "lists/blocks_list.c"

int32_t blocks_setup(void) {
    for (block_id i=0; i<BLOCKS_COUNT; i++) {
        blocks[i].slot = assets_find(blocks[i].kind);
    }
    blocks__chunk_tbl_init(&baked_chunks, zpl_heap());
    return 0;
}

void blocks_destroy(void) {
    blocks__chunk_tbl_map_mut(&baked_chunks, chunks_unload_textures);
    blocks__chunk_tbl_destroy(&baked_chunks);
}

block_id blocks_find(asset_id kind) {
    for (block_id i=0; i<BLOCKS_COUNT; i++) {
        if (blocks[i].kind == kind)
            return i;
    }
    return 0xF;
}

block_id blocks_find_by_symbol(char symbol) {
    for (block_id i=0; i<BLOCKS_COUNT; i++) {
        if (blocks[i].symbol == symbol)
            return i;
    }
    return 0xF;
}

asset_id blocks_get_asset(block_id id) {
    return blocks[id].kind;
}

char blocks_get_symbol(block_id id) {
    return blocks[id].symbol;
}

uint32_t blocks_get_flags(block_id id) {
    return blocks[id].flags;
}

float blocks_get_drag(block_id id) {
    return blocks[id].drag;
}

float blocks_get_friction(block_id id) {
    return blocks[id].friction;
}

float blocks_get_bounce(block_id id) {
    return blocks[id].bounce;
}

float blocks_get_velx(block_id id) {
    return blocks[id].velx;
}

float blocks_get_vely(block_id id) {
    return blocks[id].vely;
}

void *blocks_get_img(block_id id) {
    return assets_get_tex(blocks[id].slot);
}

void blocks_build_chunk_tex(uint64_t id, block_id *chunk_blocks, void *raw_view) {
    world_view *view = (world_view*)raw_view;
    uint16_t blk_dims = (uint16_t)(WORLD_BLOCK_SIZE * WORLD_TEXTURE_BLOCK_SCALE);
    uint16_t dims = blk_dims * view->chunk_size;
    RenderTexture2D canvas = LoadRenderTexture(dims, dims);
    BeginTextureMode(canvas);
    ClearBackground(WHITE);
    for (int y = 0; y < view->chunk_size; y += 1) {
        for (int x = 0; x < view->chunk_size; x += 1) {
            static float rots[] = { 0.0f, 90.0f, 180.f, 270.0f };
            float rot = rots[(int32_t)(perlin_fbm(view->seed, x, y, 1.2f, 3) * 4.0f) % 4];
            float half_block = blk_dims / 2.0f;
            Texture2D blk = *(Texture2D*)blocks_get_img(chunk_blocks[(y*view->chunk_size)+x]);
            static Rectangle src = {0, 0, WORLD_BLOCK_SIZE, WORLD_BLOCK_SIZE};
            Rectangle dst = {x*blk_dims + half_block, y*blk_dims + half_block, blk_dims, blk_dims};
            DrawTexturePro(blk, src, dst, (Vector2){half_block, half_block}, rot, WHITE);
        }
    }
    EndTextureMode();
    SetTextureWrap(canvas.texture, TEXTURE_WRAP_CLAMP);
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
