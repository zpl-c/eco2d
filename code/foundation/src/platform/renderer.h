#pragma once
#include "platform/system.h"
#include "world/entity_view.h"
#include "world/blocks.h"
#include "raylib.h"

typedef struct {
    uint64_t key;
    entity_view *data;
    float x;
    float y;
    float cy;
    block_id blk_id;
} game_world_render_entry;

void renderer_draw(void);
void renderer_init(void);
void renderer_shutdown(void);
void renderer_debug_draw(void);
float renderer_zoom_get(void);
void renderer_draw_single(float x, float y, asset_id id, Color color);
void renderer_draw_entry(uint64_t key, entity_view * data, game_world_render_entry* entry);
void renderer_bake_chunk(uint64_t key, entity_view * data);
void renderer_switch(int kind);
