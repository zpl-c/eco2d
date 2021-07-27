#include "zpl.h"
#include "modules/systems.h"
#include "modules/components.h"
#include "world/world.h"
#include "world/blocks.h"
#include "profiler.h"

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

#define PHY_PUSHOUT_DIST ((64.0f*WORLD_BLOCK_SIZE))

void PushOutOverlappingEntities(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    
    for (int i = 0; i <= it->count; i++) {
#if 1
        // NOTE(zaklaus): slow path. iterate over all the entities in the table.
        for (int k = 0; k <= it->count; k++) {
            if (i == k) continue;
#else
            // TODO(zaklaus): use a shared internal buffer instead !!!
            static int64_t ents[UINT32_MAX];
            size_t ents_count = UINT32_MAX;
            librg_world_fetch_chunk(world_tracker(), librg_chunk_from_realpos(world_tracker(), p[i].x, p[i].y, 0), ents, &ents_count);
            
            // NOTE(zaklaus): iterate over all entities inside this chunk
            for (size_t j = 0; j < ents_count; j++) {
                ecs_entity_t e = ents[j];
                
                if (e == it->entities[i])
                    continue;
                
                // NOTE(zaklaus): reverse lookup
                int k = 0;
                for (; k <= it->count; k++) {
                    if (k == it->count) {
                        k = -1;
                        break;
                    }
                    if (it->entities[k] == e) {
                        break;
                    }
                }
                
                if (k == -1)
                    continue;
#endif
                
                float dx = p[i].x - p[k].x;
                float dy = p[i].y - p[k].y;
                float dist = zpl_sqrt(dx*dx + dy*dy);
                if (dist < PHY_PUSHOUT_DIST) {
                    p[i].x = zpl_sign(dx);
                    p[i].y = zpl_sign(dy);
#if 0
                    p[k].x += zpl_sign(dx);
                    p[k].y += zpl_sign(dy);
#endif
                }
            }
        }
    }
    
#if 0
}
#endif

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

void SystemsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Systems);
    ECS_IMPORT(ecs, Components);
    
    ECS_SYSTEM(ecs, MovementImpulse, EcsOnLoad, components.Input, components.Velocity);
    ECS_SYSTEM(ecs, DemoPlaceIceBlock, EcsOnLoad, components.Input, components.Position);
    ECS_SYSTEM(ecs, DemoNPCMoveAround, EcsOnLoad, components.Velocity, components.EcsDemoNPC);
    
    ECS_SYSTEM(ecs, MoveWalk, EcsOnUpdate, components.Position, components.Velocity);
    
    ECS_SYSTEM(ecs, IntegratePositions, EcsOnValidate, components.Position, components.Velocity);
    //ECS_SYSTEM(ecs, PushOutOverlappingEntities, EcsOnValidate, components.Position, Velocity);
    
    ECS_SYSTEM(ecs, UpdateTrackerPos, EcsPostUpdate, components.Position);
}
