#pragma once
#include "system.h"

typedef enum {
    PROF_TOTAL_TIME,
    PROF_MAIN_LOOP,
    
    PROF_WORLD_WRITE,
    PROF_RENDER,
    PROF_UPDATE_SYSTEMS,
    PROF_ENTITY_LERP,
    PROF_ENTITY_REMOVAL,
    PROF_INTEGRATE_POS,
    
    MAX_PROF,
    PROF_FORCE_UINT8 = UINT8_MAX
} profiler_kind;

typedef struct {
    profiler_kind id;
    char const *name;
    
    uint32_t num_invocations;
    double start_time;
    double delta_time;
    double total_time;
} profiler;

void profiler_reset(profiler_kind id);
void profiler_start(profiler_kind id);
void profiler_stop(profiler_kind id);
void profiler_collate(void);

double profiler_delta(profiler_kind id);
char const *profiler_name(profiler_kind id);

#define profile(id) defer(profiler_start(id), profiler_stop(id))
