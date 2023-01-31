#include "zpl.h"
#include "systems/systems.h"
#include "models/components.h"
#include "world/world.h"
#include "world/blocks.h"
#include "platform/profiler.h"
#include "dev/debug_draw.h"
#include "core/game.h"
#include "core/rules.h"
#include "ferox.h"

extern frWorld *phys_world;

#define PHY_BLOCK_COLLISION 1
#define PHY_WALK_DRAG 4.23f
#define PHY_LOOKAHEAD(x) (zpl_sign(x)*16.0f)

#include "modules/system_onfoot.c"
#include "modules/system_demo.c"
#include "modules/system_vehicle.c"
#include "modules/system_items.c"
#include "modules/system_logistics.c"
#include "modules/system_producer.c"
#include "modules/system_blueprint.c"
#include "modules/system_mob.c"

static inline float physics_correction(float x, float vx, float bounce, float dim) {
    float r = (((zpl_max(0.0f, dim - zpl_abs(x))*zpl_sign(x)))*dim);
    return r + (-vx*bounce);
}

static inline bool physics_check_aabb(float a1x, float a2x, float a1y, float a2y, float b1x, float b2x, float b1y, float b2y) {
    return (a1x < b2x && a2x > b1x && a1y < b2y && a2y > b1y);
}

void IntegratePositions(ecs_iter_t *it) {
    profile(PROF_INTEGRATE_POS) {
        Position *p = ecs_field(it, Position, 1);
        Velocity *v = ecs_field(it, Velocity, 2);
        
        for (int i = 0; i < it->count; i++) {
            if (ecs_get(it->world, it->entities[i], IsInVehicle)) {
                continue;
            }

            //if (zpl_abs(v[i].x) >= 0.0001f || zpl_abs(v[i].y) >= 0.0001f)
            {
                // NOTE(zaklaus): world bounds
                {
                    float w = (float)world_dim();
                    p[i].x = zpl_clamp(p[i].x, 0, w-1);
                    p[i].y = zpl_clamp(p[i].y, 0, w-1);
                }
                
#if PHY_BLOCK_COLLISION==1
                // NOTE(zaklaus): X axis
                {
                    world_block_lookup lookup = world_block_from_realpos(p[i].x+PHY_LOOKAHEAD(v[i].x), p[i].y);
                    uint32_t flags = blocks_get_flags(lookup.bid);
                    float bounce = blocks_get_bounce(lookup.bid);
                    if (flags & BLOCK_FLAG_COLLISION && physics_check_aabb(p[i].x-WORLD_BLOCK_SIZE/4, p[i].x+WORLD_BLOCK_SIZE/4, p[i].y-0.5f, p[i].y+0.5f, lookup.aox-WORLD_BLOCK_SIZE/2, lookup.aox+WORLD_BLOCK_SIZE/2, lookup.aoy-WORLD_BLOCK_SIZE/2, lookup.aoy+WORLD_BLOCK_SIZE/2)) {
#if 1
                        {
                            debug_v2 a = {p[i].x-WORLD_BLOCK_SIZE/4 + PHY_LOOKAHEAD(v[i].x), p[i].y-0.5f};
                            debug_v2 b = {p[i].x+WORLD_BLOCK_SIZE/4 + PHY_LOOKAHEAD(v[i].x), p[i].y+0.5f};
                            debug_push_rect(a, b, 0xFF0000FF);
                        }
#endif
                        v[i].x = physics_correction(lookup.ox, v[i].x, bounce, WORLD_BLOCK_SIZE/2);
                    }
                }
                
                // NOTE(zaklaus): Y axis
                {
                    world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y+PHY_LOOKAHEAD(v[i].y));
                    uint32_t flags = blocks_get_flags(lookup.bid);
                    float bounce = blocks_get_bounce(lookup.bid);
#if 0
                    {
                        debug_v2 a = {lookup.aox-WORLD_BLOCK_SIZE/2, lookup.aoy-WORLD_BLOCK_SIZE/2};
                        debug_v2 b = {lookup.aox+WORLD_BLOCK_SIZE/2, lookup.aoy+WORLD_BLOCK_SIZE/2};
                        debug_push_rect(a, b, 0xFFFFFFFF);
                    }
#endif
                    if (flags & BLOCK_FLAG_COLLISION && physics_check_aabb(p[i].x-WORLD_BLOCK_SIZE/4, p[i].x+WORLD_BLOCK_SIZE/4, p[i].y-0.5f, p[i].y+0.5f, lookup.aox-WORLD_BLOCK_SIZE/2, lookup.aox+WORLD_BLOCK_SIZE/2, lookup.aoy-WORLD_BLOCK_SIZE/2, lookup.aoy+WORLD_BLOCK_SIZE/2)) {
#if 1
                        {
                            debug_v2 a = {p[i].x-WORLD_BLOCK_SIZE/4, p[i].y-0.5f + PHY_LOOKAHEAD(v[i].y)};
                            debug_v2 b = {p[i].x+WORLD_BLOCK_SIZE/4, p[i].y+0.5f + PHY_LOOKAHEAD(v[i].y)};
                            debug_push_rect(a, b, 0xFF0000FF);
                        }
#endif
                        v[i].y = physics_correction(lookup.oy, v[i].y, bounce, WORLD_BLOCK_SIZE/4);
                    }
                }
#endif
                
                entity_set_position(it->entities[i], p[i].x+v[i].x*safe_dt(it), p[i].y+v[i].y*safe_dt(it));
            }
            
            {
                debug_v2 a = {p[i].x, p[i].y};
                debug_v2 b = {p[i].x+v[i].x, p[i].y+v[i].y};
                debug_push_line(a, b, 0xFFFFFFFF);
            }
            
            {
                debug_v2 a = {p[i].x-WORLD_BLOCK_SIZE/4, p[i].y-0.5f};
                debug_v2 b = {p[i].x+WORLD_BLOCK_SIZE/4, p[i].y+0.5f};
                debug_push_rect(a, b, 0xFFFFFFFF);
            }
        }
    }
}

#define HAZARD_BLOCK_DMG 5.0f

void HurtOnHazardBlock(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    Health *h = ecs_field(it, Health, 2);
    
    for (int i = 0; i < it->count; i++) {
		world_block_lookup l = world_block_from_realpos(p[i].x, p[i].y);
		if (blocks_get_flags(l.bid) & BLOCK_FLAG_HAZARD) {
			h->hp -= HAZARD_BLOCK_DMG;
			h->hp = zpl_max(0.0f, h->hp);
			ecs_add(it->world, it->entities[i], HealthDecreased);
		}
    }
}

//#define HP_REGEN_PAIN_COOLDOWN 5.0f

void RegenerateHP(ecs_iter_t *it) {
	Health *h = ecs_field(it, Health, 1);
	HealthRegen *r = ecs_field(it, HealthRegen, 2);
    
    for (int i = 0; i < it->count; i++) {
		// TODO delay regen on hurt
		if (h[i].hp < h[i].max_hp) {
			h[i].hp += r->amt;
			h[i].hp = zpl_min(h[i].max_hp, h[i].hp);
			entity_wake(it->entities[i]);
		}
    }
}

void OnHealthChangePutDelay(ecs_iter_t *it) {
	for (int i = 0; i < it->count; i++) {
		ecs_set(it->world, it->entities[i], HealDelay, { .delay = 10 });
		ecs_remove(it->world, it->entities[i], HealthDecreased);
	}
}

void TickDownHealDelay(ecs_iter_t *it) {
	HealDelay *h = ecs_field(it, HealDelay, 1);
    
	for (int i = 0; i < it->count; i++) {
		--h[i].delay;

		if (h[i].delay == 0) {
			ecs_remove(it->world, it->entities[i], HealDelay);
		}
	}
}

void ResetActivators(ecs_iter_t *it) {
    Input *in = ecs_field(it, Input, 1);
    
    for (int i = 0; i < it->count; i++) {
        in[i].use = false;
        in[i].swap = false;
        in[i].drop = false;
        in[i].pick = false;
        in[i].num_placements = 0;
    }
}

void ApplyWorldDragOnVelocity(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (zpl_abs(v[i].x) < 0.001f && zpl_abs(v[i].y) < 0.001f) continue;
        if (ecs_get(it->world, it->entities[i], IsInVehicle)) {
            continue;
        }
        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = zpl_clamp(blocks_get_drag(lookup.bid), 0.0f, 1.0f);
        float friction = blocks_get_friction(lookup.bid);
        float velx = blocks_get_velx(lookup.bid);
        float vely = blocks_get_vely(lookup.bid);
        v[i].x = zpl_lerp(v[i].x, zpl_max(0.0f, zpl_abs(velx))*zpl_sign(velx), PHY_WALK_DRAG*drag*friction*safe_dt(it));
        v[i].y = zpl_lerp(v[i].y, zpl_max(0.0f, zpl_abs(vely))*zpl_sign(vely), PHY_WALK_DRAG*drag*friction*safe_dt(it));
        
        if (   zpl_abs(v[i].x) > ENTITY_ACTION_VELOCITY_THRESHOLD
            || zpl_abs(v[i].y) > ENTITY_ACTION_VELOCITY_THRESHOLD) {
            entity_wake(it->entities[i]);
        }
    }
}

#define PLAYER_MAX_INTERACT_RANGE 35.0f

void PlayerClosestInteractable(ecs_iter_t *it){
    Input *in = ecs_field(it, Input, 1);
    
    for (int i = 0; i < it->count; ++i) {
        size_t ents_count;
        int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);
        
        ecs_entity_t closest_pick = 0;
        float min_pick = ZPL_F32_MAX;
        
        for (size_t j = 0; j < ents_count; j++) {
            const Position *p2 = ecs_get(it->world, ents[j], Position);
            if (!p2) continue;
            
            float dx = p2->x - in[i].bx;
            float dy = p2->y - in[i].by;
            float range = zpl_sqrt(dx*dx + dy*dy);
            if (range <= PLAYER_MAX_INTERACT_RANGE && range < min_pick) {
                min_pick = range;
                closest_pick = ents[j];
            }
        }
        
        in[i].pick_ent = closest_pick;
        
        if (in[i].pick)
            in[i].sel_ent = (in[i].sel_ent == closest_pick) ? 0 : closest_pick;
    }
}

void PhysOnCreateBody(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);
	Position *p = ecs_field(it, Position, 2);

	for (int i = 0; i < it->count; i++) {
		const frMaterial mat = {
			.density = pb[i].density,
			.staticFriction = pb[i].static_friction,
			.dynamicFriction = pb[i].dynamic_friction,
		};

		frShape *shape = 0;
		if (pb[i].kind == PHYS_CIRCLE) {
			shape = frCreateCircle(mat, pb[i].circle.r);
		} else {
			shape = frCreateRectangle(mat, pb[i].rect.w, pb[i].rect.h);
		}

		frBodyFlags flags = 0x0;
		if (pb[i].inf_inertia) flags |= FR_FLAG_INFINITE_INERTIA;
		if (pb[i].inf_mass) flags |= FR_FLAG_INFINITE_MASS;
		frBody *body = frCreateBodyFromShape(FR_BODY_DYNAMIC, flags, frVec2PixelsToMeters((Vector2){p[i].x, p[i].y}), shape);
		frAddToWorld(phys_world, body);
		pb[i].body_ptr = (uintptr_t)body;
	}
}

void PhysOnRemoveBody(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);

	for (int i = 0; i < it->count; i++) {
		frBody *body = (frBody*)pb[i].body_ptr;
		frRemoveFromWorld(phys_world, body);
		frShape *shape = frGetBodyShape(body);
		frReleaseBody(body);
		frReleaseShape(shape);
	}
}

void PhysSetVelocity(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);
	Velocity *v = ecs_field(it, Velocity, 2);

	for (int i = 0; i < it->count; i++) {
		frBody *body = (frBody*)pb[i].body_ptr;
		frSetBodyVelocity(body, (Vector2) { v[i].x, v[i].y });
	}
}

void PhysUpdatePosition(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);
	Position *p = ecs_field(it, Position, 2);
	Velocity *v = ecs_field(it, Velocity, 3);

	for (int i = 0; i < it->count; i++) {
		frBody *body = (frBody*)pb[i].body_ptr;
		Vector2 pos = frVec2MetersToPixels(frGetBodyPosition(body));
		p[i].x = pos.x;
		p[i].y = pos.y;
		Vector2 vel = frVec2MetersToPixels(frGetBodyVelocity(body));
		v[i].x = vel.x;
		v[i].y = vel.y;
	}
}

void PhysResetPosition(ecs_iter_t *it) {
	Position *p = ecs_field(it, Position, 1);

	for (int i = 0; i < it->count; i++) {
		const PhysicsBody *pb = ecs_get(it->world, it->entities[i], PhysicsBody);
		if (!pb) continue; 
		frBody *body = (frBody*)pb->body_ptr;
		frSetBodyPosition(body, (Vector2){p[i].x, p[i].y});
	}
}

void PhysSimulateWorld(ecs_iter_t *it) {
	frSimulateWorld(phys_world, it->delta_time);
}

void EnableWorldEdit(ecs_iter_t *it) {
    world_set_stage(it->world);
}

void DisableWorldEdit(ecs_iter_t *it) {
    (void)it;
    world_set_stage(NULL);
}

#define ECO2D_TICK_RATE (1.0f/20.f)

#define ECS_SYSTEM_TICKED(world, id, stage, ...)\
ECS_SYSTEM(world, id, stage, __VA_ARGS__);\
ecs_set_tick_source(world, id, timer);

#define ECS_SYSTEM_TICKED_EX(world, id, stage, time, ...)\
ECS_SYSTEM(world, id, stage, __VA_ARGS__);\
ecs_entity_t timer_##id = ecs_set_interval(ecs, 0, ECO2D_TICK_RATE*time);\
ecs_set_tick_source(world, id, timer_##id);

void SystemsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Systems);
    
    ecs_entity_t timer = ecs_set_interval(ecs, 0, ECO2D_TICK_RATE);

	ECS_SYSTEM(ecs, EnableWorldEdit, EcsOnLoad);
    ECS_SYSTEM(ecs, MovementImpulse, EcsOnLoad, components.Input, components.Velocity, components.Position, !components.IsInVehicle);
    ECS_SYSTEM(ecs, DemoNPCMoveAround, EcsOnLoad, components.Velocity, components.DemoNPC);
    
    ECS_SYSTEM(ecs, ApplyWorldDragOnVelocity, EcsOnUpdate, components.Position, components.Velocity);
	ECS_SYSTEM_TICKED_EX(ecs, HurtOnHazardBlock, EcsOnUpdate, 20.0f, components.Position, components.Health);
	ECS_SYSTEM_TICKED_EX(ecs, RegenerateHP, EcsOnUpdate, 40.0f, components.Health, components.HealthRegen, !components.HealDelay);
	ECS_SYSTEM_TICKED_EX(ecs, TickDownHealDelay, EcsOnUpdate, 20.0f, components.HealDelay);
    ECS_SYSTEM(ecs, VehicleHandling, EcsOnUpdate, components.Vehicle, components.Position, components.Velocity);

	ECS_OBSERVER(ecs, OnHealthChangePutDelay, EcsOnAdd, components.HealthDecreased);
    
	ECS_SYSTEM(ecs, PhysSetVelocity, EcsOnValidate, components.PhysicsBody, components.Velocity);
	ECS_SYSTEM(ecs, PhysSimulateWorld, EcsOnValidate);
	ECS_SYSTEM(ecs, PhysUpdatePosition, EcsOnValidate, components.PhysicsBody, components.Position, components.Velocity);
	ECS_SYSTEM(ecs, IntegratePositions, EcsOnValidate, components.Position, components.Velocity, !components.PhysicsBody);
    
    ECS_SYSTEM(ecs, EnterVehicle, EcsPostUpdate, components.Input, components.Position, !components.IsInVehicle);
    ECS_SYSTEM(ecs, LeaveVehicle, EcsPostUpdate, components.Input, components.IsInVehicle, components.Velocity);
    
    ECS_SYSTEM(ecs, PlayerClosestInteractable, EcsPostUpdate, components.Input);
    ECS_SYSTEM(ecs, PickItem, EcsPostUpdate, components.Input, components.Position, components.Inventory, !components.IsInVehicle);
    ECS_SYSTEM(ecs, DropItem, EcsPostUpdate, components.Input, components.Position, components.Inventory, !components.IsInVehicle);
    ECS_SYSTEM(ecs, SwapItems, EcsPostUpdate, components.Input, components.Inventory);
    //ECS_SYSTEM(ecs, MergeItems, EcsPostUpdate, components.Position, components.ItemDrop);
    ECS_SYSTEM(ecs, UseItem, EcsPostUpdate, components.Input, components.Position, components.Inventory, !components.IsInVehicle);
    ECS_SYSTEM(ecs, CraftItem, EcsPostUpdate, components.Input, !components.IsInVehicle);
    ECS_SYSTEM(ecs, InspectContainers, EcsPostUpdate, components.Input, !components.IsInVehicle);
    
    ECS_SYSTEM_TICKED(ecs, HarvestIntoContainers, EcsPostUpdate, components.ItemContainer, components.Position, !components.BlockHarvest);
    ECS_SYSTEM_TICKED(ecs, ProduceItems, EcsPostUpdate, components.ItemContainer, components.Producer, components.Position, components.Device);
    ECS_SYSTEM_TICKED_EX(ecs, PushItemsOnNodes, EcsPostUpdate, 20, components.ItemContainer, components.Position, components.Device, components.ItemRouter);
	ECS_SYSTEM_TICKED(ecs, BuildBlueprints, EcsPostUpdate, components.Blueprint, components.Device, components.Position);
	
	ECS_SYSTEM_TICKED(ecs, CreatureCheckNeeds, EcsPostUpdate, components.Creature);
	ECS_SYSTEM_TICKED(ecs, CreatureSeekFood, EcsPostUpdate, components.Creature, components.Position, components.Velocity, components.SeeksFood, !components.SeeksCompanion);
	ECS_SYSTEM_TICKED(ecs, CreatureSeekCompanion, EcsPostUpdate, components.Creature, components.Position, components.Velocity, components.SeeksCompanion, !components.SeeksFood);
	ECS_SYSTEM(ecs, CreatureRoamAround, EcsPostUpdate, components.Velocity, components.Creature, !components.SeeksFood, !components.SeeksCompanion);

	ECS_SYSTEM_TICKED_EX(ecs, MobDetectPlayers, EcsPostUpdate, 100.0f, components.Position, components.Mob);
	ECS_SYSTEM(ecs, MobMovement, EcsPostUpdate, components.Velocity, components.Position, components.MobHuntPlayer);
	ECS_SYSTEM_TICKED(ecs, MobMeleeAtk, EcsPostUpdate, components.Position, components.Mob, components.MobHuntPlayer, components.MobMelee);
    
    ECS_SYSTEM(ecs, ResetActivators, EcsPostUpdate, components.Input);
    
    ECS_SYSTEM(ecs, ClearVehicle, EcsUnSet, components.Vehicle);
    ECS_SYSTEM(ecs, ThrowItemsOut, EcsUnSet, components.ItemContainer, components.Position);

	// Physics hooks
	ECS_OBSERVER(ecs, PhysOnCreateBody, EcsOnSet, components.PhysicsBody, components.Position);
	ECS_OBSERVER(ecs, PhysOnRemoveBody, EcsUnSet, components.PhysicsBody);
	ECS_OBSERVER(ecs, PhysResetPosition, EcsOnSet, components.Position);
    
    ECS_SYSTEM(ecs, DisableWorldEdit, EcsPostUpdate);
    
}
