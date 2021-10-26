#pragma once
#include "flecs/flecs.h"

typedef struct {
    // NOTE(zaklaus): Public systems are exposed here
    int32_t _unused;
} Systems;

#define SystemsImportHandles(handles) (void)(handles)

void SystemsImport(ecs_world_t *ecs);
