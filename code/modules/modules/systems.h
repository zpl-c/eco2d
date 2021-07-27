#pragma once
#include "flecs/flecs.h"

typedef struct {
    // NOTE(zaklaus): Public systems are exposed here
} Systems;

#define SystemsImportHandles(handles) (void)(handles)

void SystemsImport(ecs_world_t *ecs);
