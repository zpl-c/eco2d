#ifndef FLECS_DASH_IMPL
#include "flecs_dash.h"
#endif


static
void EcsUpdateFps(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsFps *world_fps = ecs_column(it, EcsFps, 1);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    world_fps->value = 1.0 / info->delta_time_raw;
    world_fps->count += world_fps->value;
    world_fps->target = info->target_fps;
}

static
void EcsUpdateLoad(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsLoad *world_load = ecs_column(it, EcsLoad, 1);

    const ecs_world_info_t *info = ecs_get_world_info(world);

    world_load->total_time_count += info->delta_time_raw;
    world_load->frame_time_count = info->frame_time_total;
    world_load->system_time_count = info->system_time_total;
    world_load->merge_time_count = info->merge_time_total;
}

static
void EcsUpdateTick(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsTick *world_tick = ecs_column(it, EcsTick, 1);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    world_tick->count = info->frame_count_total;
}

static
void EcsUpdateClock(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsWorldClock *world_clock = ecs_column(it, EcsWorldClock, 1);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    world_clock->world_time = info->world_time_total;
    world_clock->world_time_raw = 0;
}

void FlecsDashMonitorImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsDashMonitor);

    ECS_IMPORT(world, FlecsMeta);

    ecs_set_name_prefix(world, "Ecs");

    ECS_META(world, EcsFps);
    ECS_META(world, EcsLoad);
    ECS_META(world, EcsTick);
    ECS_META(world, EcsWorldClock);

    /* System for keeping track of world stats */
    ecs_set(world, EcsWorld, EcsFps, {0});
    ECS_SYSTEM(world, EcsUpdateFps, EcsOnLoad, flecs.core.World:Fps);   

    ecs_set(world, EcsWorld, EcsLoad, {0});
    ECS_SYSTEM(world, EcsUpdateLoad, EcsOnLoad, flecs.core.World:Load); 

    ecs_set(world, EcsWorld, EcsTick, {0});
    ECS_SYSTEM(world, EcsUpdateTick, EcsOnLoad, flecs.core.World:Tick);  

    ecs_set(world, EcsWorld, EcsWorldClock, {0});
    ECS_SYSTEM(world, EcsUpdateClock, EcsOnLoad, flecs.core.World:WorldClock);
}

static
bool request_this(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_entity_t server = ecs_get_parent_w_entity(world, entity, 0);
    if (server) {
        ecs_strbuf_t buf = ECS_STRBUF_INIT;
        char *path = ecs_get_fullpath(world, server);
        ecs_strbuf_append(&buf, "{\"server_id\":\"%s\"}", path);
        ecs_os_free(path);
        reply->body = ecs_strbuf_get(&buf);
        return true;
    } else {
        reply->body = ecs_os_strdup("{\"server_id\":\"unknown\"}");
        return false;
    }
}

static
bool request_files(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    const char *file = request->relative_url;
    char path[1024];

    if (!file || !strlen(file)) {
        file = "index.html";
    }

    char *etc_path = ecs_os_module_to_etc("flecs.dash");

    sprintf(path, "%s/%s", etc_path, file);

    FILE *f = fopen(path, "r");
    if (!f) {
        return false;
    } else {
        fclose(f);
    }

    reply->body = ecs_os_strdup(path);
    reply->is_file = true;

    ecs_os_free(etc_path);

    return true;
}

static
bool request_player(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    const char *cmd = request->relative_url;

    ecs_entity_t ecs_typeid(EcsPlayer) = ecs_lookup_fullpath(
            world, "flecs.player.Player");

    if (ecs_typeid(EcsPlayer)) {
        EcsPlayer *player = ecs_get_mut(world, EcsWorld, EcsPlayer, NULL);

        if (!strcmp(cmd, "play")) {
            player->state = EcsPlayerPlay;
        } else if (!strcmp(cmd, "pause")) {
            player->state = EcsPlayerPause;
        } else if (!strcmp(cmd, "stop")) {
            player->state = EcsPlayerStop;
        }

        ecs_modified(world, EcsWorld, EcsPlayer);
    }

    return true;
}

static
void RunServer(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsDashServer *server = ecs_column(it, EcsDashServer, 1);

    ecs_entity_t ecs_typeid(EcsHttpEndpoint) = ecs_column_entity(it, 2);
    ecs_entity_t ecs_typeid(EcsRestServer) = ecs_column_entity(it, 3);
    ecs_entity_t ecs_typeid(EcsMonitorServer) = ecs_column_entity(it, 4);
    ecs_entity_t ecs_typeid(EcsDashApp) = ecs_column_entity(it, 5);
    ecs_entity_t EcsDashInitialized = ecs_column_entity(it, 6);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        EcsDashServer *s = &server[i];

        /* Create REST & monitor server */
        ecs_set(world, e, EcsRestServer, {.port = s->port});
        ecs_set(world, e, EcsMonitorServer, {.port = s->port});

        if (ecs_has_entity(world, e, EcsDashInitialized)) {
            /* Don't add endpoints again if already initialized */
            continue;
        }
        
        /* Add endpoint to server for serving up files */
        ecs_entity_t e_files = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, e_files, EcsName, {"e_files"});
            ecs_set(world, e_files, EcsHttpEndpoint, {
                .url = "",
                .action = request_files});

        /* Add endpoint to server that returns entity id of server */
        ecs_entity_t e_this = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, e_this, EcsName, {"e_this"});
            ecs_set(world, e_this, EcsHttpEndpoint, {
                .url = "this",
                .action = request_this}); 

        /* Add endpoint to server that returns entity id of server */
        ecs_entity_t e_player = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, e_player, EcsName, {"e_player"});
            ecs_set(world, e_player, EcsHttpEndpoint, {
                .url = "player",
                .action = request_player,
                .synchronous = true});

        /* Add browser app */
        ecs_entity_t dash_overview = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, dash_overview, EcsName, {"overview"});
            ecs_set(world, dash_overview, EcsDashApp, {
                .path = "etc/apps/overview",
                .icon = "images/usage.png"
            });

        ecs_entity_t dash_systems = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, dash_systems, EcsName, {"systems"});
            ecs_set(world, dash_systems, EcsDashApp, {
                .path = "etc/apps/systems",
                .icon = "images/layers.png"
            });  

        ecs_entity_t dash_browser = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, dash_browser, EcsName, {"browser"});
            ecs_set(world, dash_browser, EcsDashApp, {
                .path = "etc/apps/browser",
                .icon = "images/tree.png"
            });            

        /* Prevent initializing the server again */
        ecs_add_entity(world, e, EcsDashInitialized);
    }
}

void FlecsDashImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsDash);

    ECS_IMPORT(world, FlecsMonitor);
    ECS_IMPORT(world, FlecsDashMonitor);

    ECS_IMPORT(world, FlecsMeta);
    ECS_IMPORT(world, FlecsPlayer);
    ECS_IMPORT(world, FlecsComponentsHttp);
    ECS_IMPORT(world, FlecsRest);

    ecs_set_name_prefix(world, "EcsDash");

    ECS_META(world, EcsDashServer);
    ECS_META(world, EcsDashApp);

    ECS_TAG(world, EcsDashInitialized);

    ECS_SYSTEM(world, RunServer, EcsOnSet, Server,
        :flecs.components.http.Endpoint,
        :flecs.rest.Server,
        :flecs.monitor.Server,
        :App,
        :Initialized);

    ECS_EXPORT_COMPONENT(EcsDashServer);
    ECS_EXPORT_COMPONENT(EcsDashApp);
}
