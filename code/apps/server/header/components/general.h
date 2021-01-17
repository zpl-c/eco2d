#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

ECS_STRUCT(Chunk, {
    int16_t x;
    int16_t y;
});

ECS_STRUCT(Position, {
    int16_t x;
    int16_t y;
});

typedef struct {
    ECS_DECLARE_COMPONENT(Chunk);
    ECS_DECLARE_COMPONENT(Position);
} Common;

#define CommonImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Chunk);\
    ECS_IMPORT_COMPONENT(handles, Position);

static inline void CommonImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Common);

    ecs_set_name_prefix(ecs, "Common");

    ECS_IMPORT(ecs, FlecsMeta);

    ECS_META(ecs, Chunk);
    ECS_META(ecs, Position);

    ECS_EXPORT_COMPONENT(Chunk);
    ECS_EXPORT_COMPONENT(Position);
}
