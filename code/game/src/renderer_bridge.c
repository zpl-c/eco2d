#include "debug_draw.h"

#if GFX_KIND == 0 || !defined(GFX_KIND)
// NOTE(zaklaus): renderer_v0
#include "renderer_v0.c"
#define renderer_init renderer_init_v0
#define renderer_shutdown renderer_shutdown_v0
#define renderer_draw renderer_draw_v0
#define renderer_debug_draw renderer_debug_draw_v0
void renderer_switch(int kind) {}
#elif GFX_KIND == 1
// NOTE(zaklaus): renderer_3d
#include "renderer_3d.c"
#define renderer_init renderer_init_3d
#define renderer_shutdown renderer_shutdown_3d
#define renderer_draw renderer_draw_3d
#define renderer_debug_draw renderer_debug_draw_3d
void renderer_switch(int kind) {}
#elif GFX_KIND == 2
#include "renderer_3d.c"
#include "renderer_v0.c"
// NOTE(zaklaus): hybrid mode
static int gfx_kind = 0; // 2d -- 0, 3d -- 1

void renderer_draw(void) {
    switch (gfx_kind) {
        case 0:{
            renderer_draw_v0();
        }break;
        case 1:{
            renderer_draw_3d();
        }break;
    }
}

void renderer_init(void) {
    switch (gfx_kind) {
        case 0:{
            renderer_init_v0();
        }break;
        case 1:{
            renderer_init_3d();
        }break;
    }
}

void renderer_shutdown(void) {
    switch (gfx_kind) {
        case 0:{
            renderer_shutdown_v0();
        }break;
        case 1:{
            renderer_shutdown_3d();
        }break;
    }
}

void renderer_debug_draw(void) {
    switch (gfx_kind) {
        case 0:{
            renderer_debug_draw_v0();
        }break;
        case 1:{
            renderer_debug_draw_3d();
        }break;
    }
}

void renderer_bake_chunk(uint64_t key, entity_view * data) {
    if (data->kind != EKIND_CHUNK) return;
    world_view *view = game_world_view_get_active();
    blocks_build_chunk_tex(key, data->blocks, data->outer_blocks, view);
}

void renderer_switch(int kind) {
    renderer_shutdown();
    gfx_kind = kind;
    renderer_init();
    
    game_world_view_active_entity_map(renderer_bake_chunk);
}
#endif