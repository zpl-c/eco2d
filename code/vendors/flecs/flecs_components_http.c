#ifndef FLECS_COMPONENTS_HTTP_IMPL
#include "flecs_components_http.h"
#endif

static
void decode(
    char *str) 
{
    char ch, *ptr, *dst = str;

    for (ptr = str; (ch = *ptr); ptr++) {
        if (ch == '%') {
            if (ptr[1] == '2' && ptr[2] == '0') {
                dst[0] = ' ';
                dst ++;
                ptr += 2;
            }
        } else {
            dst[0] = ptr[0];
            dst ++;
        }
    }
    
    dst[0] = '\0';
}

size_t ecs_http_get_query_param(
    const char *query,
    const char *name, 
    char *buffer, 
    size_t len)
{
    if (!query) {
        return 0;
    }
    
    const char *cur, *start = query, *value = NULL;
    char ch;
    int arg_len = strlen(name);

    for (cur = query; (ch = *cur); cur ++) {
        if (ch == '=') {
            if (cur - start >= 256) {
                /* Ignore, key is too long */
            } else {
                int32_t key_len = cur - start;
                cur ++;
                value = cur;
                while ((ch = *cur) && ch != '&') {
                    cur ++;
                }

                if (!ch || ch == '&') {
                    /* Check if requested key matches */
                    if (arg_len == key_len && !strncmp(name, start, key_len)) {
                        size_t size;
                        if (cur - value < (int32_t)len) {
                            size = cur - value;
                        } else {
                            size = len - 1;
                        }

                        memcpy(buffer, value, size);
                        buffer[size] = '\0';

                        decode(buffer);

                        return cur - value;
                    }
                }

                start = cur + 1;
            }
        }
    }

    return 0;
}

void FlecsComponentsHttpImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsComponentsHttp);

    ECS_IMPORT(world, FlecsMeta);
    
    ecs_set_name_prefix(world, "EcsHttp");

    ECS_META(world, EcsHttpServer);
    ECS_META(world, EcsHttpEndpoint);

    ECS_SET_COMPONENT(EcsHttpServer);
    ECS_SET_COMPONENT(EcsHttpEndpoint);
}
