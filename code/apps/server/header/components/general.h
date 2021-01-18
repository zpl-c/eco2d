#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

#include "assets.h"

ECS_STRUCT(Vector2D, {
    int16_t x;
    int16_t y;
});

ECS_STRUCT(Drawable, {
    uint16_t id;
});

ECS_ALIAS(Vector2D, Chunk);
ECS_ALIAS(Vector2D, Position);

typedef struct {
    ECS_DECLARE_COMPONENT(Chunk);
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_COMPONENT(Vector2D);
    ECS_DECLARE_COMPONENT(Drawable);
} General;

#define GeneralImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Chunk);\
    ECS_IMPORT_COMPONENT(handles, Vector2D);\
    ECS_IMPORT_COMPONENT(handles, Position);\
    ECS_IMPORT_COMPONENT(handles, Drawable);\

static inline void GeneralImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, General);
    ecs_set_name_prefix(ecs, "General");

    ECS_COMPONENT(ecs, Chunk);
    ECS_COMPONENT(ecs, Position);

    ECS_IMPORT(ecs, FlecsMeta);

    ECS_META(ecs, Vector2D);
    ECS_META(ecs, Drawable);

    ECS_SET_COMPONENT(Chunk);
    ECS_SET_COMPONENT(Vector2D);
    ECS_SET_COMPONENT(Position);
    ECS_SET_COMPONENT(Drawable);
}
