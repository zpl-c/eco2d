#include "zpl.h"
#include "systems/systems.h"
#include "models/components.h"
#include "world/world.h"
#include "world/blocks.h"
#include "platform/profiler.h"
#include "dev/debug_draw.h"
#include "core/game.h"
#include "core/rules.h"

#include "packets/pkt_send_notif.h"

ZPL_DIAGNOSTIC_PUSH_WARNLEVEL(0)
#define CUTE_C2_IMPLEMENTATION
#include "tinyc2.h"
ZPL_DIAGNOSTIC_POP

#define PHY_BLOCK_COLLISION 1
#define PHY_C2_BLOCK_COLLISION 0
#define PHY_WALK_DRAG 4.23f
#define PHY_LOOKAHEAD(x) (zpl_sign(x)*16.0f)

ecs_query_t *ecs_rigidbodies = 0;
ecs_entity_t ecs_timer = 0;

#include "modules/system_onfoot.c"
#include "modules/system_health.c"
#include "modules/system_demo.c"
#include "modules/system_vehicle.c"
#include "modules/system_items.c"
#include "modules/system_logistics.c"
#include "modules/system_producer.c"
#include "modules/system_blueprint.c"

static inline float physics_correction(float x, float vx, float bounce, float dim) {
    float r = (((zpl_max(0.0f, dim - zpl_abs(x))*zpl_sign(x)))*dim);
    return r + (-vx*bounce);
}

static inline bool physics_check_aabb(float a1x, float a2x, float a1y, float a2y, float b1x, float b2x, float b1y, float b2y) {
    return (a1x < b2x && a2x > b1x && a1y < b2y && a2y > b1y);
}

static inline bool BlockCollisionIslandTest(Position *p, librg_chunk ch_p) {
	// collect islands
	collision_island islands[16];
	uint8_t num_islands = world_chunk_collision_islands(ch_p, islands);
	for (uint8_t i = 0; i < num_islands; i++) {
#if 1
		{
			zpl_printf("px %f py %f minx %f miny %f\n", p->x, p->y, islands[i].minx, islands[i].miny);
			debug_v2 a = {islands[i].minx, islands[i].miny};
			debug_v2 b = {islands[i].maxx+WORLD_BLOCK_SIZE, islands[i].maxy+WORLD_BLOCK_SIZE};
			debug_push_rect(a, b, 0xFFFFFFFF);
		}
#endif		
	}

	return 0;
}

void BlockCollisions(ecs_iter_t *it) {
	profile(PROF_PHYS_BLOCK_COLS) {
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

#if PHY_C2_BLOCK_COLLISION==1
				// collision islands
				{
					librg_chunk chunk_id = librg_chunk_from_realpos(world_tracker(), p[i].x, p[i].y, 0);

					if (BlockCollisionIslandTest((p+i), chunk_id))
						continue;
				}
#endif
                
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
			}
		}
	}
}

void BodyCollisions(ecs_iter_t *it) {
	Position *p = ecs_field(it, Position, 1);
	Velocity *v = ecs_field(it, Velocity, 2);
	PhysicsBody *b = ecs_field(it, PhysicsBody, 3);

	profile(PROF_PHYS_BODY_COLS) {
		for (int i = 0; i < it->count; i++) {
			if (ecs_get(it->world, it->entities[i], IsInVehicle)) {
				continue;
			}

#if 0
			{
				debug_v2 a = {p[i].x - WORLD_BLOCK_SIZE/2, p[i].y - WORLD_BLOCK_SIZE/2};
				debug_v2 b = {p[i].x + WORLD_BLOCK_SIZE/2, p[i].y + WORLD_BLOCK_SIZE/2};
				debug_push_rect(a, b, 0xFFFFFFFF);
			}
#endif

			ecs_iter_t it2 = ecs_query_iter(it->world, ecs_rigidbodies);

			while (ecs_query_next(&it2)) {
				for (int j = 0; j < it2.count; j++) {
					if (it->entities[i] == it2.entities[j]) continue;

					Position *p2 = ecs_field(&it2, Position, 1);
					Velocity *v2 = ecs_field(&it2, Velocity, 2);
					PhysicsBody *b2 = ecs_field(&it2, PhysicsBody, 3);

					float p_x = p[i].x;
					float p_y = p[i].y;
					float p2_x = p2[j].x /*+ v2[j].x*/;
					float p2_y = p2[j].y /*+ v2[j].y*/;
	
					c2AABB box_a = {
						.min = { p_x - WORLD_BLOCK_SIZE / 2, p_y - WORLD_BLOCK_SIZE / 4 },
						.max = { p_x + WORLD_BLOCK_SIZE / 2, p_y + WORLD_BLOCK_SIZE / 4 },
					};
	
					c2AABB box_b = {
						.min = { p2_x - WORLD_BLOCK_SIZE / 2, p2_y - WORLD_BLOCK_SIZE / 4 },
						.max = { p2_x + WORLD_BLOCK_SIZE / 2, p2_y + WORLD_BLOCK_SIZE / 4 },
					};

					// do a basic sweep first
					float r1x = (box_a.max.x-box_a.min.x);
					float r1y = (box_a.max.y-box_a.min.y);
					float r1 = (r1x*r1x + r1y*r1y)*.5f;

					float r2x = (box_b.max.x-box_b.min.x);
					float r2y = (box_b.max.y-box_b.min.y);
					float r2 = (r2x*r2x + r2y*r2y)*.5f;

					{
						float dx = (p2_x-p_x);
						float dy = (p2_y-p_y);
						float d = (dx*dx + dy*dy);

						if (d > r1 && d > r2)
							continue;
					}

					c2Circle circle_a = { 
						.p = { p_x, p_y },
						.r = r1/2.f,
					};

					c2Circle circle_b = {
						.p = { p2_x, p2_y },
						.r = r2/2.f,
					};

					const void *shapes_a[] = { &circle_a, &box_a };
					const void *shapes_b[] = { &circle_b, &box_b };
					
					c2Manifold m = { 0 };
					c2Collide(shapes_a[b[i].kind], 0, b[i].kind, shapes_b[b2[j].kind], 0, b2[j].kind, &m);

					c2v n = m.n;
	
					for (int k = 0; k < m.count; k++) {
						float d = m.depths[k];
#if 0
						{
							c2v pos = m.contact_points[k];
							debug_v2 a = { pos.x, pos.y };
							debug_v2 b = { pos.x + n.x*d, pos.y + n.y*d };
							debug_push_line(a, b, 0xF77FFFFF);
						}
#endif
						
						float m1 = b2[j].mass == INFINITE_MASS ? b[i].mass : b2[j].mass;
						float m2 = b[i].mass == INFINITE_MASS ? (ZPL_F32_MAX-1.0f) : b[i].mass;
						float mass_ratio = m1 / m2;

						v[i].x -= n.x*d*mass_ratio;
						v[i].y -= n.y*d*mass_ratio;
					}
				}
			}
		}
	}
}

void IntegratePositions(ecs_iter_t *it) {
    profile(PROF_INTEGRATE_POS) {
        Position *p = ecs_field(it, Position, 1);
        Velocity *v = ecs_field(it, Velocity, 2);
        
        for (int i = 0; i < it->count; i++) {
            if (ecs_get(it->world, it->entities[i], IsInVehicle)) {
                continue;
            }

			entity_set_position(it->entities[i], p[i].x+v[i].x*safe_dt(it), p[i].y+v[i].y*safe_dt(it));
            
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

void EnableWorldEdit(ecs_iter_t *it) {
    world_set_stage(it->world);
}

void DisableWorldEdit(ecs_iter_t *it) {
    (void)it;
    world_set_stage(NULL);
}

void SystemsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Systems);
    
    ecs_timer = ecs_set_interval(ecs, 0, ECO2D_TICK_RATE);

	ecs_rigidbodies = ecs_query_new(ecs, "components.Position, components.Velocity, components.PhysicsBody");

	ECS_SYSTEM(ecs, EnableWorldEdit, EcsOnLoad);
    
	// health
	ECS_SYSTEM_TICKED_EX(ecs, HurtOnHazardBlock, EcsOnUpdate, 20.0f, components.Position, components.Health);
	ECS_SYSTEM_TICKED_EX(ecs, RegenerateHP, EcsOnUpdate, 40.0f, components.Health, components.HealthRegen, !components.HealDelay);
	ECS_SYSTEM_TICKED_EX(ecs, TickDownHealDelay, EcsOnUpdate, 20.0f, components.HealDelay);
	ECS_OBSERVER(ecs, OnHealthChangePutDelay, EcsOnAdd, components.HealthDecreased);
	ECS_OBSERVER(ecs, OnHealthChangeCheckDead, EcsOnAdd, components.HealthDecreased);
	ECS_OBSERVER(ecs, OnDead, EcsOnAdd, components.Dead);
    
	// collisions and movement physics
	ECS_SYSTEM(ecs, ApplyWorldDragOnVelocity, EcsOnUpdate, components.Position, components.Velocity);
	ECS_SYSTEM(ecs, VehicleHandling, EcsOnUpdate, components.Vehicle, components.Position, components.Velocity);
	ECS_SYSTEM(ecs, BodyCollisions, EcsOnUpdate, components.Position, components.Velocity, components.PhysicsBody);
	ECS_SYSTEM(ecs, BlockCollisions, EcsOnValidate, components.Position, components.Velocity);
	ECS_SYSTEM(ecs, IntegratePositions, EcsOnValidate, components.Position, components.Velocity);
    
	// vehicles
    ECS_SYSTEM(ecs, EnterVehicle, EcsPostUpdate, components.Input, components.Position, !components.IsInVehicle);
    ECS_SYSTEM(ecs, LeaveVehicle, EcsPostUpdate, components.Input, components.IsInVehicle, components.Velocity);
    
	// player interaction
	ECS_SYSTEM(ecs, MovementImpulse, EcsOnLoad, components.Input, components.Velocity, components.Position, !components.IsInVehicle);
	ECS_SYSTEM(ecs, PlayerClosestInteractable, EcsPostUpdate, components.Input);
    ECS_SYSTEM(ecs, PickItem, EcsPostUpdate, components.Input, components.Position, components.Inventory, !components.IsInVehicle);
    ECS_SYSTEM(ecs, DropItem, EcsPostUpdate, components.Input, components.Position, components.Inventory, !components.IsInVehicle);
    ECS_SYSTEM(ecs, SwapItems, EcsPostUpdate, components.Input, components.Inventory);
    //ECS_SYSTEM(ecs, MergeItems, EcsPostUpdate, components.Position, components.ItemDrop);
    ECS_SYSTEM(ecs, UseItem, EcsPostUpdate, components.Input, components.Position, components.Inventory, !components.IsInVehicle);
    ECS_SYSTEM(ecs, CraftItem, EcsPostUpdate, components.Input, !components.IsInVehicle);
    ECS_SYSTEM(ecs, InspectContainers, EcsPostUpdate, components.Input, !components.IsInVehicle);
    
	// logistics and production
    ECS_SYSTEM_TICKED(ecs, HarvestIntoContainers, EcsPostUpdate, components.ItemContainer, components.Position, !components.BlockHarvest);
    ECS_SYSTEM_TICKED(ecs, ProduceItems, EcsPostUpdate, components.ItemContainer, components.Producer, components.Position, components.Device);
    ECS_SYSTEM_TICKED_EX(ecs, PushItemsOnNodes, EcsPostUpdate, 20, components.ItemContainer, components.Position, components.Device, components.ItemRouter);
	ECS_SYSTEM_TICKED(ecs, BuildBlueprints, EcsPostUpdate, components.Blueprint, components.Device, components.Position);
	
	// demo creature sim
	ECS_SYSTEM_TICKED(ecs, CreatureCheckNeeds, EcsPostUpdate, components.Creature);
	ECS_SYSTEM_TICKED(ecs, CreatureSeekFood, EcsPostUpdate, components.Creature, components.Position, components.Velocity, components.SeeksFood, !components.SeeksCompanion);
	ECS_SYSTEM_TICKED(ecs, CreatureSeekCompanion, EcsPostUpdate, components.Creature, components.Position, components.Velocity, components.SeeksCompanion, !components.SeeksFood);
	ECS_SYSTEM(ecs, CreatureRoamAround, EcsPostUpdate, components.Velocity, components.Creature, !components.SeeksFood, !components.SeeksCompanion);
	
	// player input reset
    ECS_SYSTEM(ecs, ResetActivators, EcsPostUpdate, components.Input);
    
	// cleanup systems
    ECS_SYSTEM(ecs, ClearVehicle, EcsUnSet, components.Vehicle);
    ECS_SYSTEM(ecs, ThrowItemsOut, EcsUnSet, components.ItemContainer, components.Position);
	
    ECS_SYSTEM(ecs, DisableWorldEdit, EcsPostUpdate);
    
}
