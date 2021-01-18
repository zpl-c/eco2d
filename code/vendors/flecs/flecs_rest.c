#ifndef FLECS_REST_IMPL
#include "flecs_rest.h"
#endif
#include <stdio.h>
#include <ctype.h>

static
bool parse_filter(
    ecs_world_t *world,
    EcsHttpRequest *request,
    ecs_filter_t *filter)
{
    char buffer[1024];

    if (ecs_http_get_query_param(
        request->params, "include", buffer, sizeof(buffer)))
    {
        filter->include = ecs_type_from_str(world, buffer);
        filter->include_kind = EcsMatchAny;
        if (!filter->include) {
            return false;
        }
    }

    if (ecs_http_get_query_param(
        request->params, "exclude", buffer, sizeof(buffer)))
    {
        filter->exclude = ecs_type_from_str(world, buffer);
        if (!filter->exclude) {
            return false;
        }
    }

    return true;
}

static
bool parse_select(
    ecs_world_t *world,
    EcsHttpRequest *request,
    ecs_type_t *select)
{
    char buffer[1024];

    *select = NULL;

    if (ecs_http_get_query_param(
        request->params, "select", buffer, sizeof(buffer)))
    {
        *select = ecs_type_from_str(world, buffer);
        if (!*select) {
            return false;
        }
    }

    return true; 
}

static
bool parse_entity(
    ecs_world_t *world,
    EcsHttpRequest *request,
    ecs_entity_t *e_out)
{
    ecs_entity_t e = 0;

    if (request->relative_url && strlen(request->relative_url)) {
        /* Request scope of entity */
        const char *name = request->relative_url;

        /* If name starts with a number, lookup by entity id */
        if (isdigit(name[0])) {
            e = atoi(name);
        } else {
            e = ecs_lookup_path_w_sep(world, 0, name, "/", NULL);
            if (!e) {
                return false;
            }
        }
    }

    *e_out = e;

    return true;
}

static
bool endpoint_filter(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_filter_t filter = { };
    ecs_type_t select = NULL;

    if (!parse_filter(world, request, &filter)) {
        return false;
    }

    if (!parse_select(world, request, &select)) {
        return false;
    }

    ecs_iter_t it = ecs_filter_iter(world, &filter);

    reply->body = ecs_iter_to_json(world, &it, ecs_filter_next, select);

    return true;
}

static
bool endpoint_scope(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_entity_t e = 0;
    ecs_filter_t filter = { };
    ecs_type_t select = NULL;
    
    if (!parse_entity(world, request, &e)) {
        return false;
    }

    if (!parse_filter(world, request, &filter)) {
        return false;
    }

    if (!parse_select(world, request, &select)) {
        return false;
    }

    ecs_iter_t it = ecs_scope_iter_w_filter(world, e, &filter);

    reply->body = ecs_iter_to_json(world, &it, ecs_scope_next, select);

    return true;
}

static
bool endpoint_entity_new(
    ecs_world_t *world,
    EcsHttpReply *reply,
    const char *name,
    ecs_type_t select)
{
    ecs_entity_t e = ecs_new_from_path(world, 0, name);
    if (select) {
        ecs_add_type(world, e, select);
    }

    return true;
}

static
bool endpoint_entity_add(
    ecs_world_t *world,
    EcsHttpReply *reply,
    ecs_entity_t e,
    ecs_type_t select)
{
    if (select) {
        ecs_add_type(world, e, select);
    }

    return true;
}

static
bool endpoint_entity_remove(
    ecs_world_t *world,
    EcsHttpReply *reply,
    ecs_entity_t e,
    ecs_type_t select)
{
    if (select) {
        ecs_remove_type(world, e, select);
    } else {
        ecs_delete(world, e);
    }

    return true;
}

static
bool endpoint_entity_get(
    ecs_world_t *world,
    EcsHttpReply *reply,
    ecs_entity_t e,
    ecs_type_t select)
{
    reply->body = ecs_entity_to_json(world, e, select);
    return true;    
}

static
bool endpoint_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_entity_t e = 0;
    ecs_type_t select = NULL;

    if (!parse_select(world, request, &select)) {
        return false;
    }

    if (request->method == EcsHttpPost) {
        return endpoint_entity_new(world, reply, request->relative_url, select);
    }    

    if (!parse_entity(world, request, &e)) {
        return false;
    }

    if (request->method == EcsHttpGet) {
        return endpoint_entity_get(world, reply, e, select);
    } else if (request->method == EcsHttpDelete) {
        return endpoint_entity_remove(world, reply, e, select);
    } else if (request->method == EcsHttpPut) {
        return endpoint_entity_add(world, reply, e, select);
    }

    return false;
}

static
void serialize_browse_info(
  ecs_world_t *world,
  ecs_entity_t parent,
  ecs_entity_t e,
  ecs_strbuf_t *str)
{
    ecs_strbuf_list_append(str, "\"id\":%u", (int32_t)e);

    char *type = ecs_type_to_json(world, ecs_get_type(world, e));
    ecs_strbuf_list_append(str, "\"type\":%s", type);
    
    const char *name = ecs_get_name(world, e);
    if (name) {
        ecs_strbuf_list_append(str, "\"name\":\"%s\"", name);

        char *fullpath = ecs_get_fullpath(world, e);
        ecs_strbuf_list_append(str, "\"path\":\"%s\"", fullpath);
    }

    int32_t child_count = ecs_get_child_count(world, e);
    ecs_strbuf_list_append(str, "\"child_count\":%u", child_count);
}

static
bool endpoint_browse(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_filter_t filter = { };
    ecs_entity_t e;

    if (!parse_entity(world, request, &e)) {
        return false;
    }

    if (!parse_filter(world, request, &filter)) {
        return false;
    }

    ecs_iter_t it = ecs_scope_iter_w_filter(world, e, &filter);
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    ecs_strbuf_list_push(&str, "[", ",");
    while (ecs_scope_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_strbuf_list_next(&str);
            ecs_strbuf_list_push(&str, "{", ",");
            serialize_browse_info(world, e, it.entities[i], &str);
            ecs_strbuf_list_pop(&str, "}");
        }
    }
    ecs_strbuf_list_pop(&str, "]");

    reply->body = ecs_strbuf_get(&str);

    return true;
}

static
bool endpoint_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_entity_t e;

    if (!parse_entity(world, request, &e)) {
        return false;
    }

    ecs_strbuf_t str = ECS_STRBUF_INIT;
    ecs_strbuf_list_push(&str, "{", ",");
    serialize_browse_info(world, e, e, &str);
    ecs_strbuf_list_pop(&str, "}");

    reply->body = ecs_strbuf_get(&str);

    return true;
}

static
void EcsRestSetServer(ecs_iter_t *it) {
    EcsRestServer *server_data = ecs_column(it, EcsRestServer, 1);
    ecs_entity_t ecs_typeid(EcsHttpServer) = ecs_column_entity(it, 2);
    ecs_entity_t ecs_typeid(EcsHttpEndpoint) = ecs_column_entity(it, 3);
    ecs_entity_t EcsRestInitialized = ecs_column_entity(it, 4);

    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t server = it->entities[i];

        // Get filtered list of entities
        ecs_set(world, server, EcsHttpServer, {.port = server_data[i].port});

        if (ecs_has_entity(world, server, EcsRestInitialized)) {
            /* Don't add endpoints more than once */
            continue;
        }

          ecs_entity_t e_filter = ecs_new_w_entity(world, ECS_CHILDOF | server);
            ecs_set(world, e_filter, EcsName, {"e_filter"});
            ecs_set(world, e_filter, EcsHttpEndpoint, {
                .url = "filter",
                .action = endpoint_filter,
                .synchronous = true,
                .ctx = NULL
            });
          
          // Get filtered list of children
          ecs_entity_t e_scope = ecs_new_w_entity(world, ECS_CHILDOF | server);
            ecs_set(world, e_scope, EcsName, {"e_scope"});
            ecs_set(world, e_scope, EcsHttpEndpoint, {
                .url = "scope",
                .action = endpoint_scope,
                .synchronous = true,
                .ctx = NULL
            });

          // Get component data for single entity
          ecs_entity_t e_entity = ecs_new_w_entity(world, ECS_CHILDOF | server);
            ecs_set(world, e_entity, EcsName, {"e_entity"});
            ecs_set(world, e_entity, EcsHttpEndpoint, {
                .url = "entity",
                .action = endpoint_entity,
                .synchronous = true,
                .ctx = NULL
            });  

          // Browse entity information
          ecs_entity_t e_browse = ecs_new_w_entity(world, ECS_CHILDOF | server);
            ecs_set(world, e_browse, EcsName, {"e_browse"});
            ecs_set(world, e_browse, EcsHttpEndpoint, {
                .url = "browse",
                .action = endpoint_browse,
                .synchronous = true,
                .ctx = NULL
            }); 

          // Browse entity information for single entity
          ecs_entity_t e_info = ecs_new_w_entity(world, ECS_CHILDOF | server);
            ecs_set(world, e_info, EcsName, {"e_info"});
            ecs_set(world, e_info, EcsHttpEndpoint, {
                .url = "info",
                .action = endpoint_info,
                .synchronous = true,
                .ctx = NULL
            });

        ecs_add_entity(world, server, EcsRestInitialized);
    }
}

void FlecsRestImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsRest);

    ecs_set_name_prefix(world, "EcsRest");

    ECS_IMPORT(world, FlecsMeta);
    ECS_IMPORT(world, FlecsComponentsHttp);

    ECS_META(world, EcsRestServer);
    ECS_TAG(world, EcsRestInitialized);

    ECS_SYSTEM(world, EcsRestSetServer, EcsOnSet, Server, 
        :flecs.components.http.Server, 
        :flecs.components.http.Endpoint,
        :Initialized);

    ECS_EXPORT_COMPONENT(EcsRestServer);
}
