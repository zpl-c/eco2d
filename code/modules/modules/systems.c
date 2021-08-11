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

#include "source/system_onfoot.c"
#include "source/system_demo.c"
#include "source/system_vehicle.c"

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

void ApplyWorldDragOnVelocity(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    
    for (int i = 0; i < it->count; i++) {
        world_block_lookup lookup = world_block_from_realpos(p[i].x, p[i].y);
        float drag = zpl_clamp(blocks_get_drag(lookup.block_id), 0.0f, 1.0f);
        float friction = blocks_get_friction(lookup.block_id);
        v[i].x = zpl_lerp(v[i].x, 0.0f, PHY_WALK_DRAG*drag*friction);
        v[i].y = zpl_lerp(v[i].y, 0.0f, PHY_WALK_DRAG*drag*friction);
    }
}

void SystemsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Systems);
    
    ECS_SYSTEM(ecs, MovementImpulse, EcsOnLoad, components.Input, components.Velocity, components.Position, !components.IsInVehicle);
    ECS_SYSTEM(ecs, DemoNPCMoveAround, EcsOnLoad, components.Velocity, components.EcsDemoNPC);
    ECS_SYSTEM(ecs, EnterVehicle, EcsOnLoad, components.Input, components.Position, !components.IsInVehicle);
    ECS_SYSTEM(ecs, LeaveVehicle, EcsOnLoad, components.Input, components.IsInVehicle);
    ECS_SYSTEM(ecs, DemoPlaceIceBlock, EcsOnLoad, components.Input, components.Position, !components.IsInVehicle);
    
    ECS_SYSTEM(ecs, ApplyWorldDragOnVelocity, EcsOnUpdate, components.Position, components.Velocity);
    ECS_SYSTEM(ecs, HurtOnHazardBlock, EcsOnUpdate, components.Position, components.Health);
    ECS_SYSTEM(ecs, RegenerateHP, EcsOnUpdate, components.Health);
    ECS_SYSTEM(ecs, VehicleHandling, EcsOnUpdate, components.Vehicle, components.Position, components.Velocity);
    
    ECS_SYSTEM(ecs, IntegratePositions, EcsOnValidate, components.Position, components.Velocity);
    
    ECS_SYSTEM(ecs, UpdateTrackerPos, EcsPostUpdate, components.Position);
    
    ECS_SYSTEM(ecs, ClearVehicle, EcsUnSet, components.Vehicle);
}
