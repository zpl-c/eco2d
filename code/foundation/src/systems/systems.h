#pragma once
#include "flecs.h"

static inline float safe_dt(ecs_iter_t *it) {
    return zpl_min(it->delta_time, 0.03334f);
}

extern ecs_query_t *ecs_rigidbodies;
extern ecs_entity_t ecs_timer;

#define TICK_VAR(var) (var) = zpl_max((var)-1, 0)

#define ECO2D_TICK_RATE (1.0f/20.f)

#define ECS_SYSTEM_TICKED(world, id, stage, ...)\
	ECS_SYSTEM(world, id, stage, __VA_ARGS__);\
	ecs_set_tick_source(world, id, ecs_timer);

#define ECS_SYSTEM_TICKED_EX(world, id, stage, time, ...)\
	ECS_SYSTEM(world, id, stage, __VA_ARGS__);\
	ecs_entity_t timer_##id = ecs_set_interval(ecs, 0, ECO2D_TICK_RATE*time);\
	ecs_set_tick_source(world, id, timer_##id);


void SystemsImport(ecs_world_t *ecs);
