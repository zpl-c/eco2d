#pragma once
#include "flecs/flecs.h"

typedef struct {
    int16_t x, y;
} chunk;

typedef struct {
    float x, y;
} position;

typedef struct {
    uint16_t peer_id;
} netclient;

ECS_COMPONENT_DECLARE(chunk);
ECS_COMPONENT_DECLARE(position);
ECS_COMPONENT_DECLARE(netclient);

static inline void components_register(ecs_world_t *ecs) {
    ECS_COMPONENT_DEFINE(ecs, chunk);
    ECS_COMPONENT_DEFINE(ecs, position);
    ECS_COMPONENT_DEFINE(ecs, netclient);
}
