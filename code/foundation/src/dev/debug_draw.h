#pragma once 
#include "platform/system.h"

// NOTE(zaklaus): Debug drawing queue

typedef enum {
    DDRAW_LINE,
    DDRAW_CIRCLE,
    DDRAW_RECT,
} debug_draw_kind;

typedef struct {
    float x,y;
} debug_v2;

typedef struct {
    debug_draw_kind kind;
    int32_t color;
    
    union {
        struct {
            debug_v2 a, b;
        };
        
        struct {
            debug_v2 bmin, bmax;
        };
        
        struct {
            debug_v2 pos;
            float radius;
        };
    };
} debug_draw_entry;

#ifndef DEBUG_DRAW_MAX_ENTRIES
#define DEBUG_DRAW_MAX_ENTRIES 65535
#endif

typedef struct {
    size_t num_entries;
    debug_draw_entry entries[DEBUG_DRAW_MAX_ENTRIES];
} debug_draw_queue;

debug_draw_queue *debug_draw_samples(void);
void debug_draw_flush(void);
void debug_draw_enable(bool state);
bool debug_draw_state(void);

void debug_push_line(debug_v2 a, debug_v2 b, int32_t color);
void debug_push_circle(debug_v2 pos, float radius, int32_t color);
void debug_push_rect(debug_v2 bmin, debug_v2 bmax, int32_t color);

