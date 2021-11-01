#include "debug_draw.h"
#include "game.h"

static debug_draw_queue draw_queue = {0};

#ifndef _DEBUG
static bool draw_is_enabled = false;
#else
static bool draw_is_enabled = true;
#endif

debug_draw_queue *debug_draw_samples(void) {
    return &draw_queue;
}

void debug_draw_flush(void) {
    draw_queue.num_entries = 0;
}

void debug_draw_enable(bool state) {
    draw_is_enabled = state;
}

bool debug_draw_state(void) {
    return draw_is_enabled;
}

static inline void debug_push_entry(debug_draw_entry entry) {
    if (!draw_is_enabled) return;
    if (game_get_kind() == GAMEKIND_HEADLESS) return;
    ZPL_ASSERT(draw_queue.num_entries < DEBUG_DRAW_MAX_ENTRIES);
    draw_queue.entries[draw_queue.num_entries++] = entry;
}

void debug_push_line(debug_v2 a, debug_v2 b, int32_t color) {
    debug_push_entry((debug_draw_entry){
                         .kind = DDRAW_LINE,
                         .color = color,
                         
                         .a = a,
                         .b = b,
                     });
}

void debug_push_circle(debug_v2 pos, float radius, int32_t color) {
    debug_push_entry((debug_draw_entry){
                         .kind = DDRAW_CIRCLE,
                         .color = color,
                         
                         .pos = pos,
                         .radius = radius,
                     });
}

void debug_push_rect(debug_v2 bmin, debug_v2 bmax, int32_t color) {
    debug_push_entry((debug_draw_entry){
                         .kind = DDRAW_RECT,
                         .color = color,
                         
                         .bmin = bmin,
                         .bmax = bmax,
                     });
}
