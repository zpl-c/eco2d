#include "zpl.h"
#include "modules/controllers.h"

#include "modules/general.h"
#include "modules/physics.h"

#include "world/blocks.h"

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

void ControllersImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Controllers);
    ecs_set_name_prefix(ecs, "Controllers");
    
    ECS_IMPORT(ecs, General);
    ECS_IMPORT(ecs, Physics);
    ECS_IMPORT(ecs, FlecsMeta);
    
    ECS_META(ecs, Input);
    
    ECS_TAG(ecs, EcsActor);
    ECS_TAG(ecs, EcsPlayer);
    ECS_TAG(ecs, EcsBuilder);
    ECS_TAG(ecs, EcsDemoNPC);
    
    ECS_SYSTEM(ecs, MovementImpulse, EcsOnLoad, Input, physics.Velocity);
    ECS_SYSTEM(ecs, DemoPlaceIceBlock, EcsOnLoad, Input, general.Position);
    ECS_SYSTEM(ecs, DemoNPCMoveAround, EcsOnLoad, physics.Velocity, EcsDemoNPC);
    
    ECS_PREFAB(ecs, Base, general.Position, physics.Velocity, Input, EcsActor);
    ECS_TYPE(ecs, Player, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Walking, EcsActor, EcsPlayer);
    ECS_TYPE(ecs, Builder, INSTANCEOF | Base, SWITCH | physics.Movement, CASE | physics.Flying, EcsActor, EcsBuilder);
    
    ECS_SET_COMPONENT(Input);
    ECS_SET_ENTITY(EcsActor);
    ECS_SET_ENTITY(EcsPlayer);
    ECS_SET_ENTITY(EcsBuilder);
    ECS_SET_ENTITY(EcsDemoNPC);
    ECS_SET_TYPE(Builder);
    ECS_SET_TYPE(Player);
}
