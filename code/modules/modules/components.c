#include "modules/components.h"

ECS_COMPONENT_DECLARE(Chunk);
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Vector2D);
ECS_COMPONENT_DECLARE(Drawable);
ECS_COMPONENT_DECLARE(Input);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(ClientInfo);
ECS_COMPONENT_DECLARE(Health);
ECS_COMPONENT_DECLARE(Classify);
ECS_COMPONENT_DECLARE(Vehicle);
ECS_TAG_DECLARE(EcsActor);
ECS_TAG_DECLARE(EcsDemoNPC);
ECS_TAG_DECLARE(EcsInVehicle);
ECS_TYPE_DECLARE(Player);
ECS_TYPE_DECLARE(Movement);
ECS_TYPE_DECLARE(Walking);
ECS_TYPE_DECLARE(Flying);
ECS_TYPE_DECLARE(EcsClient);

// NOTE(zaklaus): custom macro to define meta components outside the current scope

#ifndef ECS_META_DEFINE
#define ECS_META_DEFINE(world, T)\
ECS_COMPONENT_DEFINE(world, T);\
ecs_new_meta(world, ecs_entity(T), &__##T##__);
#endif

void ComponentsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Components);
    ECS_IMPORT(ecs, FlecsMeta);
    
    ECS_META_DEFINE(ecs, Position);
    ECS_META_DEFINE(ecs, Chunk);
    ECS_META_DEFINE(ecs, Vector2D);
    ECS_META_DEFINE(ecs, Drawable);
    ECS_META_DEFINE(ecs, ClientInfo);
    ECS_META_DEFINE(ecs, Velocity);
    ECS_META_DEFINE(ecs, Input);
    ECS_META_DEFINE(ecs, Health);
    ECS_META_DEFINE(ecs, Classify);
    ECS_META_DEFINE(ecs, Vehicle);
    
    ECS_TAG_DEFINE(ecs, Walking);
    ECS_TAG_DEFINE(ecs, Flying);
    ECS_TAG_DEFINE(ecs, EcsClient);
    
    ECS_TAG_DEFINE(ecs, EcsActor);
    ECS_TAG_DEFINE(ecs, EcsDemoNPC);
    ECS_TAG_DEFINE(ecs, EcsInVehicle);
    
    ECS_PREFAB(ecs, Base, Position, Velocity, Input, EcsActor);
    ECS_TYPE_DEFINE(ecs, Movement, Walking, Flying);
    
    ECS_SET_COMPONENT(Chunk);
    ECS_SET_COMPONENT(Vector2D);
    ECS_SET_COMPONENT(Position);
    ECS_SET_COMPONENT(Drawable);
    ECS_SET_COMPONENT(Velocity);
    ECS_SET_COMPONENT(ClientInfo);
    ECS_SET_COMPONENT(Input);
    ECS_SET_COMPONENT(Health);
    ECS_SET_COMPONENT(Classify);
    ECS_SET_COMPONENT(Vehicle);
    ECS_SET_ENTITY(EcsClient);
    ECS_SET_ENTITY(Walking);
    ECS_SET_ENTITY(Flying);
    ECS_SET_ENTITY(EcsActor);
    ECS_SET_ENTITY(EcsDemoNPC);
    ECS_SET_ENTITY(EcsInVehicle);
    ECS_SET_TYPE(Movement);
}
