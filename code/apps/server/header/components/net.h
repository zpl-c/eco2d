#pragma once
#include "flecs/flecs.h"
#include "flecs/flecs_meta.h"

ECS_STRUCT(NetClient, {
    uint16_t peer_id;
});

typedef struct {
    ECS_DECLARE_COMPONENT(NetClient);
} Net;

#define NetImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, NetClient);\

static inline void NetImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Net);

    ecs_set_name_prefix(ecs, "Net");

    ECS_IMPORT(ecs, FlecsMeta);

    ECS_META(ecs, NetClient);

    ECS_EXPORT_COMPONENT(NetClient);
}
