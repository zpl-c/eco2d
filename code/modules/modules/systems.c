#include "zpl.h"
#include "modules/systems.h"
#include "modules/components.h"
#include "world/world.h"
#include "world/blocks.h"
#include "profiler.h"
#include "game.h"

#define PHY_BLOCK_COLLISION 1
#define PHY_WALK_DRAG 0.12
#define PHY_LOOKAHEAD(x) (zpl_sign(x)*16.0f)
#define PHY_CORRECTION(x) ((zpl_max(0.0f, (WORLD_BLOCK_SIZE/2.0f) - zpl_abs(x))*zpl_sign(x)))*(WORLD_BLOCK_SIZE/2.0f)

void MoveWalk(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    
    for (int i = 0; i < it->count; i++) {
        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = blocks_get_drag(lookup.block_id);
        v[i].x = zpl_lerp(v[i].x, 0.0f, PHY_WALK_DRAG*drag);
        v[i].y = zpl_lerp(v[i].y, 0.0f, PHY_WALK_DRAG*drag);
    }
}

void IntegratePositions(ecs_iter_t *it) {
    profile(PROF_INTEGRATE_POS) {
        Position *p = ecs_column(it, Position, 1);
        Velocity *v = ecs_column(it, Velocity, 2);
        
        for (int i = 0; i < it->count; i++) {
            // NOTE(zaklaus): world bounds
            {
                double w = (double)world_dim();
                p[i].x = zpl_clamp(p[i].x, 0, w-1);
                p[i].y = zpl_clamp(p[i].y, 0, w-1);
            }
            
#if PHY_BLOCK_COLLISION==1
            // NOTE(zaklaus): X axis
            {
                world_block_lookup lookup = world_block_from_realpos(p[i].x+PHY_LOOKAHEAD(v[i].x), p[i].y);
                uint32_t flags = blocks_get_flags(lookup.block_id);
                if (flags & BLOCK_FLAG_COLLISION) {
                    v[i].x = PHY_CORRECTION(lookup.ox);
                }
            }
            
            // NOTE(zaklaus): Y axis
            {
                world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y+PHY_LOOKAHEAD(v[i].y));
                uint32_t flags = blocks_get_flags(lookup.block_id);
                if (flags & BLOCK_FLAG_COLLISION) {
                    v[i].y = PHY_CORRECTION(lookup.oy);
                }
            }
#endif
            
            p[i].x += v[i].x * it->delta_time;
            p[i].y += v[i].y * it->delta_time;
        }
    }
}

void UpdateTrackerPos(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    
    for (int i = 0; i < it->count; i++){
        librg_entity_chunk_set(world_tracker(), it->entities[i], librg_chunk_from_realpos(world_tracker(), p[i].x, p[i].y, 0));
    }
}

#define PLR_MOVE_SPEED 50.0
#define PLR_MOVE_SPEED_MULT 4.0

void MovementImpulse(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (ecs_is_alive(world_ecs(), in[i].parent)) continue;
        double speed = PLR_MOVE_SPEED * (in[i].sprint ? PLR_MOVE_SPEED_MULT : 1.0);
        if (zpl_abs(v[i].x) < speed && in[i].x)
            v[i].x = in[i].x*speed;
        if (zpl_abs(v[i].y) < speed && in[i].y)
            v[i].y = in[i].y*speed;
    }
}

#define DEMO_NPC_CHANGEDIR_FACTOR 0.1
#define DEMO_NPC_MOVE_SPEED 1500

void DemoNPCMoveAround(ecs_iter_t *it) {
    Velocity *v = ecs_column(it, Velocity, 1);
    for (int i = 0; i < it->count; i++) {
        v[i].x = zpl_lerp(v[i].x, (rand()%3-1)*DEMO_NPC_MOVE_SPEED, DEMO_NPC_CHANGEDIR_FACTOR);
        v[i].y = zpl_lerp(v[i].y, (rand()%3-1)*DEMO_NPC_MOVE_SPEED, DEMO_NPC_CHANGEDIR_FACTOR);
    }
}

void DemoPlaceIceBlock(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Position *p = ecs_column(it, Position, 2);
    uint8_t watr_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);
    
    for (int i = 0; i < it->count; i++) {
        if (in[i].use) {
            world_block_lookup l = world_block_from_realpos(p[i].x, p[i].y);
            world_chunk_replace_block(l.chunk_id, l.id, watr_id); 
        }
    }
}

#define HAZARD_BLOCK_TIME 1.0f
#define HAZARD_BLOCK_DMG 5.0f

void HurtOnHazardBlock(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Health *h = ecs_column(it, Health, 2);
    
    for (int i = 0; i < it->count; i++) {
        world_block_lookup l = world_block_from_realpos(p[i].x, p[i].y);
        if (blocks_get_flags(l.block_id) & BLOCK_FLAG_HAZARD) {
            if (h->pain_time < game_time()) {
                h->pain_time = game_time() + HAZARD_BLOCK_TIME;
                h->hp -= HAZARD_BLOCK_DMG;
                h->hp = zpl_max(0.0f, h->hp);
            }
        }
    }
}

#define HP_REGEN_TIME 2.0f
#define HP_REGEN_PAIN_COOLDOWN 5.0f
#define HP_REGEN_RECOVERY 15.0f

void RegenerateHP(ecs_iter_t *it) {
    Health *h = ecs_column(it, Health, 1);
    
    for (int i = 0; i < it->count; i++) {
        if (h->pain_time < game_time() - HP_REGEN_PAIN_COOLDOWN) {
            if (h->heal_time < game_time() && h->hp < h->max_hp) {
                h->heal_time = game_time() + HP_REGEN_TIME;
                h->hp += HP_REGEN_RECOVERY;
                h->hp = zpl_min(h->max_hp, h->hp);
            }
        }
    }
}

#define VEH_ENTER_RADIUS 45.0f

void EnterOrLeaveVehicle(ecs_iter_t *it) {
    Input *in = ecs_column(it, Input, 1);
    Position *p = ecs_column(it, Position, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (!in[i].use) continue;
        
        if (!ecs_is_alive(world_ecs(), in[i].parent)) {
            size_t ents_count;
            int64_t *ents = world_chunk_query_entities(it->entities[i], &ents_count, 2);
            
            for (size_t j = 0; j < ents_count; j++) {
                if (ecs_get(world_ecs(), ents[j], Vehicle)) {
                    Vehicle *veh = ecs_get_mut(world_ecs(), ents[j], Vehicle, NULL);
                    Position const* p2 = ecs_get(world_ecs(), ents[j], Position);
                    
                    float dx = p2->x - p[i].x;
                    float dy = p2->y - p[i].y;
                    float range = zpl_sqrt(dx*dx + dy*dy);
                    if (range <= VEH_ENTER_RADIUS) {
                        for (int k = 0; k < 4; k++) {
                            if (veh->seats[k] != 0) continue;
                            
                            // NOTE(zaklaus): We can enter the vehicle, yay!
                            veh->seats[k] = it->entities[i];
                            in[i].parent = ents[j];
                            p[i] = *p2;
                            break;
                        }
                    }
                }
            }
        } else {
            if (ecs_get(world_ecs(), in[i].parent, Vehicle)) {
                Vehicle *veh = ecs_get_mut(world_ecs(), in[i].parent, Vehicle, NULL);
                
                for (int k = 0; k < 4; k++) {
                    if (veh->seats[k] == it->entities[i]) {
                        veh->seats[k] = 0;
                        break;
                    }
                }
                
                in[i].parent = 0;
            } else {
                ZPL_PANIC("unreachable code");
            }
        }
    }
}

#define VEHICLE_MAX_SPEED 500.0f
#define VEHICLE_ACCEL 2.1f
#define VEHICLE_STEER 0.01f

void VehicleHandling(ecs_iter_t *it) {
    Vehicle *veh = ecs_column(it, Vehicle, 1);
    Position *p = ecs_column(it, Position, 2);
    Velocity *v = ecs_column(it, Velocity, 3);
    
    for (int i = 0; i < it->count; i++) {
        Vehicle *car = &veh[i];
        
        // NOTE(zaklaus): Apply friction
        car->speed *= 0.99f;
        car->steer *= 0.97f;
        
        for (int j = 0; j < 4; j++) {
            // NOTE(zaklaus): Perform seat cleanup
            if (!ecs_is_alive(world_ecs(), veh[i].seats[j])) {
                veh[i].seats[j] = 0;
                continue;
            }
            
            ecs_entity_t pe = veh[i].seats[j];
            
            // NOTE(zaklaus): Update passenger position
            {
                Position *p2 = ecs_get_mut(world_ecs(), pe, Position, NULL);
                Velocity *v2 = ecs_get_mut(world_ecs(), pe, Velocity, NULL);
                *p2 = p[i];
                *v2 = v[i];
            }
            
            // NOTE(zaklaus): Handle driver input
            if (j == 0) {
                Input const* in = ecs_get(world_ecs(), pe, Input);
                
                car->speed += in->y * VEHICLE_ACCEL;
                car->speed = zpl_clamp(car->speed, -VEHICLE_MAX_SPEED, VEHICLE_MAX_SPEED);
                car->steer += in->x * -VEHICLE_STEER * (zpl_abs(car->speed*2.5f) / VEHICLE_MAX_SPEED);
                car->steer = zpl_clamp(car->steer, -40.0f, 40.0f);
            }
        }
        
        // NOTE(zaklaus): Vehicle physics
        float fr_x = p[i].x + (car->wheel_base/2.0f) * zpl_cos(car->heading);
        float fr_y = p[i].y + (car->wheel_base/2.0f) * zpl_sin(car->heading);
        
        float bk_x = p[i].x - (car->wheel_base/2.0f) * zpl_cos(car->heading);
        float bk_y = p[i].y - (car->wheel_base/2.0f) * zpl_sin(car->heading);
        
        bk_x += car->speed * zpl_cos(car->heading);
        bk_y += car->speed * zpl_sin(car->heading);
        fr_x += car->speed * zpl_cos(car->heading + zpl_to_radians(car->steer));
        fr_y += car->speed * zpl_sin(car->heading + zpl_to_radians(car->steer));
        
        v[i].x = (fr_x + bk_x) / 2.0f - p[i].x;
        v[i].y = (fr_y + bk_y) / 2.0f - p[i].y;
        car->heading = zpl_arctan2(fr_y - bk_y, fr_x - bk_x);
    }
}

void SystemsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Systems);
    ECS_IMPORT(ecs, Components);
    
    ECS_SYSTEM(ecs, MovementImpulse, EcsOnLoad, components.Input, components.Velocity);
    //ECS_SYSTEM(ecs, DemoPlaceIceBlock, EcsOnLoad, components.Input, components.Position);
    ECS_SYSTEM(ecs, DemoNPCMoveAround, EcsOnLoad, components.Velocity, components.EcsDemoNPC);
    ECS_SYSTEM(ecs, EnterOrLeaveVehicle, EcsOnLoad, components.Input, components.Position);
    
    ECS_SYSTEM(ecs, MoveWalk, EcsOnUpdate, components.Position, components.Velocity, !components.Vehicle);
    ECS_SYSTEM(ecs, HurtOnHazardBlock, EcsOnUpdate, components.Position, components.Health);
    ECS_SYSTEM(ecs, RegenerateHP, EcsOnUpdate, components.Health);
    ECS_SYSTEM(ecs, VehicleHandling, EcsOnUpdate, components.Vehicle, components.Position, components.Velocity);
    
    ECS_SYSTEM(ecs, IntegratePositions, EcsOnValidate, components.Position, components.Velocity);
    
    ECS_SYSTEM(ecs, UpdateTrackerPos, EcsPostUpdate, components.Position);
}
