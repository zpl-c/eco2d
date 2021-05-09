#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"
#include "world/world.h"

ECS_STRUCT(Vector2D, {
    float x;
    float y;
});

ECS_STRUCT(Chunk, {
    int16_t x;
    int16_t y;
    ecs_vector(uint8_t) blocks;
});

ECS_STRUCT(Drawable, {
    uint16_t id;
});

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

void GeneralImport(ecs_world_t *ecs);
