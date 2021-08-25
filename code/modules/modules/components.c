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
ECS_COMPONENT_DECLARE(IsInVehicle);
ECS_COMPONENT_DECLARE(ItemDrop);
ECS_TAG_DECLARE(EcsActor);
ECS_TAG_DECLARE(EcsDemoNPC);
ECS_TYPE_DECLARE(Player);
ECS_TYPE_DECLARE(Movement);
ECS_TYPE_DECLARE(Walking);
ECS_TYPE_DECLARE(Flying);
// NOTE(zaklaus): @1 DECLARE

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
    
    ECS_COMPONENT_DEFINE(ecs, IsInVehicle);
    ECS_COMPONENT_DEFINE(ecs, ItemDrop);
    
    ECS_TAG_DEFINE(ecs, Walking);
    ECS_TAG_DEFINE(ecs, Flying);
    
    ECS_TAG_DEFINE(ecs, EcsActor);
    ECS_TAG_DEFINE(ecs, EcsDemoNPC);
    
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
    ECS_SET_COMPONENT(IsInVehicle);
    ECS_SET_COMPONENT(ItemDrop);
    ECS_SET_ENTITY(Walking);
    ECS_SET_ENTITY(Flying);
    ECS_SET_ENTITY(EcsActor);
    ECS_SET_ENTITY(EcsDemoNPC);
    ECS_SET_TYPE(Movement);
    // NOTE(zaklaus): @2 SET
}
