
#include "zpl.h"
#include "modules/physics.h"
#include "world/world.h"
#include "world/blocks.h"
#include "profiler.h"

#define PHY_BLOCK_COLLISION 1
#define PHY_WALK_DRAG 0.12
#define PHY_LOOKAHEAD(x) (zpl_sign(x)*16.0f)
#define PHY_CORRECTION(x) ((zpl_max(0.0f, (WORLD_BLOCK_SIZE/2.0f) - zpl_abs(x))*zpl_sign(x)))*(WORLD_BLOCK_SIZE/2.0f)

void MoveWalk(ecs_iter_t *it) {
    Velocity *v = ecs_column(it, Velocity, 1);
    
    for (int i = 0; i < it->count; i++) {
        v[i].x = zpl_lerp(v[i].x, 0.0f, PHY_WALK_DRAG);
        v[i].y = zpl_lerp(v[i].y, 0.0f, PHY_WALK_DRAG);
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

void PhysicsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Physics);
    ecs_set_name_prefix(ecs, "Physics");
    
    ECS_TAG(ecs, Walking);
    ECS_TAG(ecs, Flying);
    ECS_TYPE(ecs, Movement, Walking, Flying);
    
    ECS_META(ecs, Velocity);
    
    ECS_SYSTEM(ecs, MoveWalk, EcsOnUpdate, Velocity);
    ECS_SYSTEM(ecs, IntegratePositions, EcsOnValidate, general.Position, Velocity);
    ECS_SYSTEM(ecs, UpdateTrackerPos, EcsPostUpdate, general.Position);
    
    ECS_SET_TYPE(Movement);
    ECS_SET_ENTITY(Walking);
    ECS_SET_ENTITY(Flying);
    ECS_SET_COMPONENT(Velocity);
    ECS_SET_ENTITY(MoveWalk);
    ECS_SET_ENTITY(UpdateTrackerPos);
    ECS_SET_ENTITY(IntegratePositions);
}
