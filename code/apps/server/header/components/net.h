#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

ECS_STRUCT(ClientInfo, {
    uint16_t peer_id;
});

typedef struct {
    ECS_DECLARE_ENTITY(EcsClient);
    ECS_DECLARE_COMPONENT(ClientInfo);
} Net;

#define NetImportHandles(handles)\
    ECS_IMPORT_ENTITY(handles, EcsClient);\
    ECS_IMPORT_COMPONENT(handles, ClientInfo);\

static inline void NetImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Net);

    ecs_set_name_prefix(ecs, "Net");

    ECS_TAG(ecs, EcsClient);

    ECS_IMPORT(ecs, FlecsMeta);

    ECS_META(ecs, ClientInfo);

    ECS_EXPORT_ENTITY(EcsClient);
    ECS_EXPORT_COMPONENT(ClientInfo);
}
