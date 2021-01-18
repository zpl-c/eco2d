#ifndef FLECS_MONITOR_IMPL
#include "flecs_monitor.h"
#endif

typedef struct WorldStats {
    ecs_world_stats_t stats;
} WorldStats;

typedef struct DeltaTime {
    ecs_gauge_t frame;
    ecs_gauge_t minute;
    int32_t t_frame;
    int32_t t_minute;
} DeltaTime;

typedef struct PipelineStats {
    ecs_pipeline_stats_t stats;
} PipelineStats;

static ECS_COMPONENT_DECLARE(WorldStats);
static ECS_COMPONENT_DECLARE(DeltaTime);
static ECS_COMPONENT_DECLARE(PipelineStats);

static void add_float_array(
    ecs_strbuf_t *r,
    int32_t t_start,
    const float *values,
    float scale)
{
    ecs_strbuf_list_push(r, "[", ",");

    int32_t i;
    for (i = 0; i < ECS_STAT_WINDOW; i ++) {
        int32_t t = (t_start + i + 1) % ECS_STAT_WINDOW;
        ecs_strbuf_list_append(r, "%f", values[t] / scale);
    }

    ecs_strbuf_list_pop(r, "]");
}

static void add_gauge(
    ecs_strbuf_t *r,
    const char *name,
    int32_t t,
    const ecs_gauge_t *m,
    bool min_max,
    float scale)
{
    ecs_strbuf_list_append(r, "\"%s\":", name);
    ecs_strbuf_list_push(r, "{", ",");
    ecs_strbuf_list_append(r, "\"avg\":");
    add_float_array(r, t, m->avg, scale);

    if (min_max) {
        ecs_strbuf_list_append(r, "\"min\":");
        add_float_array(r, t, m->min, scale);
        ecs_strbuf_list_append(r, "\"max\":");
        add_float_array(r, t, m->max, scale);
    }

    ecs_strbuf_list_pop(r, "}");
}

static void add_counter(
    ecs_strbuf_t *r,
    const char *name,
    int32_t t,
    const ecs_counter_t *m,
    bool min_max,
    float scale)
{
    add_gauge(r, name, t, &m->rate, min_max, scale);        
}

#define add_current(r, name, t, m, scale)\
    _add_current(r, name, t, (ecs_gauge_t*)m, scale)

static void _add_current(
    ecs_strbuf_t *r,
    const char *name,
    int32_t t,
    const ecs_gauge_t *m,
    float scale)    
{
    ecs_strbuf_list_append(r, "\"%s\":%f", name, m->avg[t] / scale);
}

static void add_world_stats(
    ecs_world_t *world,
    const ecs_world_info_t *info,
    ecs_strbuf_t *r)
{
    const WorldStats *s = ecs_get(world, ecs_typeid(WorldStats), WorldStats);
    if (!s) {
        return;
    }

    const DeltaTime *s_dt = ecs_get(world, ecs_typeid(DeltaTime), DeltaTime);
    if (!s_dt) {
        return;
    }    

    const ecs_world_stats_t *stats = &s->stats;
    int32_t t = stats->t;
    
    float df = stats->frame_count_total.rate.avg[t];
    if (df == 0.0) {
        return;
    }

    ecs_strbuf_list_appendstr(r, "\"world\":");
    ecs_strbuf_list_push(r, "{", ",");

    ecs_strbuf_list_appendstr(r, "\"current\":");
    ecs_strbuf_list_push(r, "{", ",");
    add_current(r, "entity_count", t, &stats->entity_count, 1.0);
    add_current(r, "component_count", t, &stats->component_count, 1.0);
    add_current(r, "query_count", t, &stats->query_count, 1.0);
    add_current(r, "system_count", t, &stats->system_count, 1.0);
    add_current(r, "table_count", t, &stats->table_count, 1.0);
    add_current(r, "empty_table_count", t, &stats->empty_table_count, 1.0);
    add_current(r, "singleton_table_count", t, &stats->singleton_table_count, 1.0);
    add_current(r, "matched_table_count", t, &stats->matched_table_count, 1.0);
    add_current(r, "matched_entity_count", t, &stats->matched_entity_count, 1.0);
    add_current(r, "systems_ran", t, &stats->systems_ran_frame, df);
    add_current(r, "new_count", t, &stats->new_count, 1.0);
    add_current(r, "bulk_new_count", t, &stats->bulk_new_count, 1.0);
    add_current(r, "delete_count", t, &stats->delete_count, 1.0);
    add_current(r, "clear_count", t, &stats->clear_count, 1.0);
    add_current(r, "add_count", t, &stats->add_count, 1.0);
    add_current(r, "remove_count", t, &stats->remove_count, 1.0);
    add_current(r, "set_count", t, &stats->set_count, 1.0);
    add_current(r, "discard_count", t, &stats->discard_count, 1.0);
    ecs_strbuf_list_pop(r, "}");

    ecs_strbuf_list_appendstr(r, "\"history_1m\":");
    ecs_strbuf_list_push(r, "{", ",");
    add_gauge(r, "fps", t, &stats->fps, false, 1.0);
    add_gauge(r, "delta_time", t, &s_dt->minute, true, 1.0);
    add_counter(r, "frame_time_total", t, &stats->frame_time_total, false, df);
    add_counter(r, "system_time_total", t, &stats->system_time_total, false, df);
    add_counter(r, "merge_time_total", t, &stats->merge_time_total, false, df);
    add_gauge(r, "entity_count", t, &stats->entity_count, false, 1.0);
    add_gauge(r, "matched_entity_count", t, &stats->matched_entity_count, false, 1.0);
    add_gauge(r, "table_count", t, &stats->table_count, false, 1.0);
    add_gauge(r, "empty_table_count", t, &stats->empty_table_count, false, 1.0);
    add_gauge(r, "singleton_table_count", t, &stats->singleton_table_count, false, 1.0);
    add_gauge(r, "matched_table_count", t, &stats->matched_table_count, false, 1.0);
    add_counter(r, "new_count", t, &stats->new_count, false, 1.0);
    add_counter(r, "bulk_new_count", t, &stats->bulk_new_count, false, 1.0);
    add_counter(r, "delete_count", t, &stats->delete_count, false, 1.0);
    add_counter(r, "clear_count", t, &stats->clear_count, false, 1.0);
    add_counter(r, "add_count", t, &stats->add_count, false, 1.0);
    add_counter(r, "remove_count", t, &stats->remove_count, false, 1.0);
    add_counter(r, "set_count", t, &stats->set_count, false, 1.0);
    add_counter(r, "discard_count", t, &stats->discard_count, false, 1.0);
    ecs_strbuf_list_pop(r, "}");

    ecs_strbuf_list_pop(r, "}");
}

static void add_signature(
    ecs_world_t *world,
    ecs_strbuf_t *r,
    ecs_entity_t system)
{
    const EcsQuery *q = ecs_get(world, system, EcsQuery);
    if (q) {
        ecs_strbuf_list_append(r, "\"signature\":");
        ecs_strbuf_list_push(r, "[", ",");

        ecs_sig_t *sig = ecs_query_get_sig(q->query);

        ecs_sig_column_t *columns = 
            ecs_vector_first(sig->columns, ecs_sig_column_t);
        int32_t i, count = ecs_vector_count(sig->columns);

        for (i = 0; i < count; i ++) {
            ecs_sig_column_t *col = &columns[i];
            
            if (col->oper_kind != EcsOperOr) {
                ecs_entity_t component = col->is.component;

                ecs_strbuf_list_next(r);
                ecs_strbuf_list_push(r, "{", ",");

                if (component & ECS_TRAIT) {
                    ecs_entity_t 
                    lo = ecs_entity_t_lo(component & ECS_COMPONENT_MASK),
                    hi = ecs_entity_t_hi(component & ECS_COMPONENT_MASK);

                    const char
                    *lo_name = ecs_get_name(world, lo),
                    *hi_name = ecs_get_name(world, hi);

                    if (!hi) {
                        ecs_strbuf_list_append(r, "\"name\":\"TRAIT | %s\"", lo_name);
                    } else {
                        if (lo_name) {
                            ecs_strbuf_list_append(r, "\"name\":\"%s FOR %s\"", 
                                hi_name, lo_name);
                        } else {
                            ecs_strbuf_list_append(r, "\"name\":\"%s FOR %u\"", 
                                hi_name, (uint32_t)lo);
                        }
                    }
                } else {
                    ecs_strbuf_list_append(r, "\"name\":\"%s\"", 
                        ecs_get_name(world, col->is.component));
                }

                ecs_entity_t actual = ecs_get_typeid(world, component);
                if (!ecs_has(world, actual, EcsComponent)) {
                    ecs_strbuf_list_append(r, "\"tag\":true");
                }

                if (col->inout_kind == EcsIn) {
                    ecs_strbuf_list_append(r, "\"const\":true");
                }

                if (col->oper_kind == EcsOperNot) {
                    ecs_strbuf_list_append(r, "\"exclude\":true");
                }

                if (col->from_kind != EcsFromOwned && 
                    col->from_kind != EcsFromAny && 
                    col->from_kind != EcsFromShared &&
                    col->from_kind != EcsFromEmpty) 
                {
                    ecs_strbuf_list_append(r, "\"ref\":true");
                }

                if (col->from_kind == EcsFromEmpty) {
                    ecs_strbuf_list_append(r, "\"empty\":true");
                }

                if (col->from_kind == EcsFromEntity && col->source == component) {
                    ecs_strbuf_list_append(r, "\"singleton\":true");
                }

                if (col->from_kind == EcsFromParent || col->from_kind == EcsCascade) {
                    ecs_strbuf_list_append(r, "\"parent\":true");
                }                

                ecs_strbuf_list_pop(r, "}");
            }
        }

        ecs_strbuf_list_pop(r, "]");
    }
}

static void add_system(
    ecs_world_t *world,
    ecs_strbuf_t *r,
    const ecs_pipeline_stats_t *stats,
    ecs_entity_t system,
    float df)
{
    ecs_system_stats_t *s = ecs_map_get(
        stats->system_stats, ecs_system_stats_t, system);
    
    ecs_strbuf_list_next(r);
    ecs_strbuf_list_push(r, "{", ",");
    ecs_strbuf_list_append(r, "\"name\":\"%s\"", ecs_get_name(world, system));

    ecs_entity_t module = ecs_get_parent_w_entity(world, system, EcsModule);
    if (module) {
        char *module_name = ecs_get_fullpath(world, module);
        ecs_strbuf_list_append(r, "\"module\":\"%s\"", module_name);
        ecs_os_free(module_name);
    }

    add_signature(world, r, system);
    
    if (s) {
        int32_t t = s->query_stats.t;
        ecs_strbuf_list_appendstr(r, "\"current\":");
        ecs_strbuf_list_push(r, "{", ",");
        add_current(r, "matched_table_count", t, &s->query_stats.matched_table_count, 1.0);
        add_current(r, "matched_empty_table_count", t, &s->query_stats.matched_empty_table_count, 1.0);
        add_current(r, "matched_entity_count", t, &s->query_stats.matched_entity_count, 1.0);
        add_current(r, "time_spent", t, &s->time_spent, df);
        add_current(r, "invoke_count", t, &s->invoke_count, df);
        add_current(r, "active", t, &s->active, 1.0);
        add_current(r, "enabled", t, &s->enabled, 1.0);
        ecs_strbuf_list_pop(r, "}");

        ecs_strbuf_list_appendstr(r, "\"history_1m\":");
        ecs_strbuf_list_push(r, "{", ",");
        add_gauge(r, "matched_table_count", t, &s->query_stats.matched_table_count, false, 1.0);
        add_gauge(r, "matched_empty_table_count", t, &s->query_stats.matched_empty_table_count, false, 1.0);
        add_gauge(r, "matched_entity_count", t, &s->query_stats.matched_entity_count, false, 1.0);
        add_counter(r, "time_spent", t, &s->time_spent, false, 1.0);
        add_counter(r, "invoke_count", t, &s->invoke_count, false, 1.0);
        add_gauge(r, "active", t, &s->active, false, 1.0);
        add_gauge(r, "enabled", t, &s->enabled, false, 1.0);
        ecs_strbuf_list_pop(r, "}");
    }

    ecs_strbuf_list_pop(r, "}");
}

static void add_pipeline_stats(
    ecs_world_t *world,
    const ecs_world_info_t *info,
    ecs_strbuf_t *r)
{
    /* Get number of frames passed in interval */
    const WorldStats *ws = ecs_get(world, ecs_typeid(WorldStats), WorldStats);
    if (!ws) {
        return;
    }

    const ecs_world_stats_t *wstats = &ws->stats;
    int32_t t = wstats->t;
    float df = wstats->frame_count_total.rate.avg[t];
    if (df == 0.0) {
        return;
    }

    const PipelineStats *s = ecs_get(world, 
        ecs_typeid(PipelineStats), PipelineStats);
    if (!s) {
        return;
    }

    const ecs_pipeline_stats_t *stats = &s->stats;
    int32_t i, count = ecs_vector_count(stats->systems);
    ecs_entity_t *systems = ecs_vector_first(stats->systems, ecs_entity_t);

    ecs_strbuf_list_appendstr(r, "\"pipeline\":");
    ecs_strbuf_list_push(r, "{", ",");

    add_current(r, "fps", t, &wstats->fps, 1.0);
    add_current(r, "frame_time_total", t, &wstats->frame_time_total, df);
    add_current(r, "system_time_total", t, &wstats->system_time_total, df);
    add_current(r, "merge_time_total", t, &wstats->merge_time_total, df);

    ecs_strbuf_list_appendstr(r, "\"systems\":");
    ecs_strbuf_list_push(r, "[", ",");
    for (i = 0; i < count; i ++) {
        if (systems[i]) {
            add_system(world, r, stats, systems[i], df);
        } else {
            /* Merge */
            ecs_strbuf_list_appendstr(r, "null");
        }
    }
    ecs_strbuf_list_pop(r, "]");

    ecs_strbuf_list_pop(r, "}");
}

static bool endpoint_world(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_strbuf_t r = ECS_STRBUF_INIT;
    
    const ecs_world_info_t *info = ecs_get_world_info(world);

    ecs_strbuf_list_push(&r, "{", ",");

    ecs_strbuf_list_append(&r, "\"target_fps\":%f", info->target_fps);

    char param[256];
    if (ecs_http_get_query_param(
        request->params, "world", param, sizeof(param)) && 
        !strcmp(param, "yes"))
    {
        add_world_stats(world, info, &r);
    }

    if (ecs_http_get_query_param(
        request->params, "pipeline", param, sizeof(param)) && 
        !strcmp(param, "yes"))
    {
        add_pipeline_stats(world, info, &r);
    }    

    ecs_strbuf_list_pop(&r, "}");

    reply->body = ecs_strbuf_get(&r);

    return true;
}

static void CollectWorldStats(ecs_iter_t *it) {
    WorldStats *s = ecs_column(it, WorldStats, 1);
    ecs_get_world_stats(it->world, &s->stats);
}

static void CollectPipelineStats(ecs_iter_t *it) {
    PipelineStats *s = ecs_column(it, PipelineStats, 1);
    ecs_get_pipeline_stats(it->world, ecs_get_pipeline(it->world), &s->stats);
}

static void RecordDeltaTime(ecs_iter_t *it) {
    DeltaTime *s = ecs_column(it, DeltaTime, 1);
    int32_t t_frame = s->t_frame;
    s->frame.min[t_frame] = it->delta_time;
    s->frame.avg[t_frame] = it->delta_time;
    s->frame.max[t_frame] = it->delta_time;
    s->t_frame = (t_frame + 1) % ECS_STAT_WINDOW;
}

static void CollectDeltaTime(ecs_iter_t *it) {
    DeltaTime *s = ecs_column(it, DeltaTime, 1);
    ecs_gauge_reduce(&s->minute, s->t_minute, &s->frame, s->t_frame);
    s->t_minute = (s->t_minute + 1) % ECS_STAT_WINDOW;
}

static void RunServer(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMonitorServer *server = ecs_column(it, EcsMonitorServer, 1);
    ecs_entity_t ecs_typeid(EcsHttpServer) = ecs_column_entity(it, 2);
    ecs_entity_t ecs_typeid(EcsHttpEndpoint) = ecs_column_entity(it, 3);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        ecs_set(world, e, EcsHttpServer, {.port = server[i].port});

        /* Add endpoint to server that returns entity id of server */
        ecs_entity_t e_metrics = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, e_metrics, EcsName, {"e_metrics"});
            ecs_set(world, e_metrics, EcsHttpEndpoint, {
                .url = "metrics",
                .action = endpoint_world,
                .synchronous = true});
    }
}

void FlecsMonitorImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsMonitor);

    ECS_IMPORT(world, FlecsMeta);
    ECS_IMPORT(world, FlecsComponentsHttp);

    ecs_set_name_prefix(world, "EcsMonitor");

    ECS_META(world, EcsMonitorServer);
    
    /* Private components */
    ECS_COMPONENT_DEFINE(world, WorldStats);
    ECS_COMPONENT_DEFINE(world, DeltaTime);
    ECS_COMPONENT_DEFINE(world, PipelineStats);

    ECS_SYSTEM(world, RunServer, EcsOnSet, Server,
        :flecs.components.http.Server,
        :flecs.components.http.Endpoint);

    ECS_SYSTEM(world, CollectWorldStats, EcsOnLoad, $WorldStats);
    ECS_SYSTEM(world, CollectPipelineStats, EcsOnLoad, $PipelineStats);
    ECS_SYSTEM(world, RecordDeltaTime, EcsOnLoad, $DeltaTime);
    ECS_SYSTEM(world, CollectDeltaTime, EcsOnLoad, $DeltaTime);

    /* Collect statistics once per second */
    ecs_set_interval(world, CollectWorldStats, 1.0);
    ecs_set_interval(world, CollectPipelineStats, 1.0);
    ecs_set_interval(world, CollectDeltaTime, 1.0);

    /* Initialize singleton entities for statistics components */
    ecs_set_ptr(world, ecs_typeid(WorldStats), WorldStats, NULL);
    ecs_set_ptr(world, ecs_typeid(PipelineStats), PipelineStats, NULL);
    ecs_set_ptr(world, ecs_typeid(DeltaTime), DeltaTime, NULL);

    ECS_EXPORT_COMPONENT(EcsMonitorServer);
    
    /* Enable system time monitoring */
    ecs_measure_system_time(world, true);
}
