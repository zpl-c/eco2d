#include "modules/components.h"

void ComponentsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Components);
    ECS_IMPORT(ecs, FlecsMeta);
    
    ECS_META(ecs, Position);
    ECS_META(ecs, Chunk);
    ECS_META(ecs, Vector2D);
    ECS_META(ecs, Drawable);
    ECS_META(ecs, ClientInfo);
    ECS_META(ecs, Velocity);
    ECS_META(ecs, Input);
    ECS_META(ecs, Health);
    ECS_META(ecs, Classify);
    ECS_META(ecs, Vehicle);
    
    ECS_TAG(ecs, Walking);
    ECS_TAG(ecs, Flying);
    ECS_TAG(ecs, EcsClient);
    
    ECS_TAG(ecs, EcsActor);
    ECS_TAG(ecs, EcsPlayer);
    ECS_TAG(ecs, EcsBuilder);
    ECS_TAG(ecs, EcsDemoNPC);
    
    ECS_PREFAB(ecs, Base, Position, Velocity, Input, EcsActor);
    ECS_TYPE(ecs, Movement, Walking, Flying);
    ECS_TYPE(ecs, Player, INSTANCEOF | Base, SWITCH | Movement, CASE | Walking, EcsActor, EcsPlayer);
    ECS_TYPE(ecs, Builder, INSTANCEOF | Base, SWITCH | Movement, CASE | Flying, EcsActor, EcsBuilder);
    
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
    ECS_SET_ENTITY(EcsPlayer);
    ECS_SET_ENTITY(EcsBuilder);
    ECS_SET_ENTITY(EcsDemoNPC);
    ECS_SET_TYPE(Movement);
    ECS_SET_TYPE(Builder);
    ECS_SET_TYPE(Player);
}
