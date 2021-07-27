#include "modules/general.h"

void GeneralImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, General);
    ecs_set_name_prefix(ecs, "General");
    
    ECS_IMPORT(ecs, FlecsMeta);
    
    ECS_META(ecs, Position);
    ECS_META(ecs, Chunk);
    ECS_META(ecs, Vector2D);
    ECS_META(ecs, Drawable);
    
    ECS_SET_COMPONENT(Chunk);
    ECS_SET_COMPONENT(Vector2D);
    ECS_SET_COMPONENT(Position);
    ECS_SET_COMPONENT(Drawable);
}
