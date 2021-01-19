#include "modules/net.h"

void NetImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Net);

    ecs_set_name_prefix(ecs, "Net");

    ECS_TAG(ecs, EcsClient);

    ECS_IMPORT(ecs, FlecsMeta);

    ECS_META(ecs, ClientInfo);

    ECS_EXPORT_ENTITY(EcsClient);
    ECS_EXPORT_COMPONENT(ClientInfo);
}
