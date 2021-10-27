#include "profiler.h"
#include "raylib.h"
#include <assert.h>

#define PROF_COLLATE_WINDOW 0.5

// NOTE(zaklaus): KEEP ORDER IN SYNC WITH profiler_kind ENUM !!!
static profiler profilers[] = {
    { .id = PROF_TOTAL_TIME, .name = "measured time" },
    { .id = PROF_MAIN_LOOP, .name = "main loop" },
    { .id = PROF_WORLD_WRITE, .name = "world write" },
    { .id = PROF_RENDER, .name = "render" },
    { .id = PROF_UPDATE_SYSTEMS, .name = "update systems" },
    { .id = PROF_ENTITY_LERP, .name = "entity lerp" },
    { .id = PROF_INTEGRATE_POS, .name = "entity movement" },
};

static_assert((sizeof(profilers)/sizeof(profilers[0])) == MAX_PROF, "mismatched profilers");

void profiler_reset(profiler_kind id) {
    profilers[id].num_invocations = 0;
    profilers[id].total_time = 0.0;
}

void profiler_start(profiler_kind id) {
    profilers[id].start_time = GetTime();
}

void profiler_stop(profiler_kind id) {
    profilers[id].num_invocations += 1;
    profilers[id].total_time += GetTime() - profilers[id].start_time;
    profilers[id].start_time = 0.0;
}

void profiler_collate() {
    static double frame_counter = 0.0;
    static uint64_t frames = 0;
    
    frame_counter += GetFrameTime();
    frames++;
    
    if (frame_counter >= PROF_COLLATE_WINDOW) {
        profilers[PROF_TOTAL_TIME].delta_time = frame_counter / (double)frames;
        
        for (uint32_t i = PROF_MAIN_LOOP; i < MAX_PROF; i += 1) {
            profiler *p = &profilers[i];
            p->delta_time = p->num_invocations == 0 ? 0.0 : p->total_time / (double)p->num_invocations;
        }
        
        frame_counter = 0.0;
        frames = 0;
    }
}

double profiler_delta(profiler_kind id) {
    return profilers[id].delta_time;
}

char const *profiler_name(profiler_kind id) {
    return profilers[id].name;
}
