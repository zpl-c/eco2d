#pragma once
#include "flecs/flecs.h"

static inline float safe_dt(ecs_iter_t *it) {
    return zpl_min(it->delta_time, 0.03334f);
}

typedef struct {
    // NOTE(zaklaus): Public systems are exposed here
    int32_t _unused;
} Systems;

#define SystemsImportHandles(handles) (void)(handles)

void SystemsImport(ecs_world_t *ecs);
