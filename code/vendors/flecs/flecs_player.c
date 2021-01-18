#ifndef FLECS_PLAYER_IMPL
#include "flecs_player.h"
#endif

static
void player_start(
    ecs_world_t *world,
    void *ctx)
{
    ecs_ref_t *ref = ctx;
    EcsPlayer *player = (EcsPlayer*)ecs_get_ref_w_entity(world, ref, 0, 0);
    ecs_assert(player != NULL, ECS_INTERNAL_ERROR, NULL);

    player->snapshot = ecs_snapshot_take(world);
    player->time_scale = 1.0;
    
    ecs_set_time_scale(world, 1.0);
    ecs_set_pipeline(world, player->play_pipeline);

    ecs_os_free(ref);
}

static
void player_stop(
    ecs_world_t *world,
    void *ctx)
{
    ecs_ref_t *ref = ctx;
    EcsPlayer *player = (EcsPlayer*)ecs_get_ref_w_entity(world, ref, 0, 0);
    ecs_assert(player != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (player->snapshot) {
        ecs_snapshot_restore(world, player->snapshot);

        player = (EcsPlayer*)ecs_get_ref_w_entity(world, ref, 0, 0);
        ecs_set_time_scale(world, 0);
        ecs_reset_clock(world);
        player->time_scale = 0;
        player->state = EcsPlayerStop;
        player->prev_state = EcsPlayerStop;
    }

    player->snapshot = NULL;
    ecs_set_pipeline(world, player->stop_pipeline);

    ecs_os_free(ref);
}

static
void EcsSetPlayer(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsPlayer *player = ecs_column(it, EcsPlayer, 1);
    ecs_entity_t ecs_typeid(EcsPlayer) = ecs_column_entity(it, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        EcsPlayerState prev_state = player[i].prev_state;
        EcsPlayerState state = player[i].state;

        if (prev_state != state) {
            switch(player[i].state) {
            case EcsPlayerPause:       
                player[i].time_scale = 0;
                break;
            case EcsPlayerPlay:
                if (prev_state == EcsPlayerStop) {
                    ecs_ref_t *ref = ecs_os_calloc(sizeof(ecs_ref_t));
                    ecs_get_ref(world, ref, it->entities[i], EcsPlayer);
                    ecs_run_post_frame(it->world, player_start, ref);
                } else {
                    player[i].time_scale = 1.0;
                }
                break;
            case EcsPlayerStop:
                if (prev_state != EcsPlayerStop) {
                    ecs_ref_t *ref = ecs_os_calloc(sizeof(ecs_ref_t));
                    ecs_get_ref(world, ref, it->entities[i], EcsPlayer);
                    ecs_run_post_frame(world, player_stop, ref);
                }
                player[i].time_scale = 0;
                break;
            default:
                break;
            }
        }    

        player[i].prev_state = state;

        ecs_set_time_scale(it->world, player[i].time_scale);    
    }
}

void FlecsPlayerImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPlayer);

    ECS_IMPORT(world, FlecsMeta);

    ecs_set_name_prefix(world, "Ecs");

    ECS_META(world, EcsPlayerState);
    ECS_META(world, EcsPlayer);
    ECS_META(world, EcsTargetFps);

    ECS_PIPELINE(world, StopPipeline, 
        flecs.pipeline.PreFrame, 
        flecs.pipeline.OnLoad, 
        flecs.pipeline.PostLoad, 
        flecs.pipeline.PreStore, 
        flecs.pipeline.OnStore, 
        flecs.pipeline.PostFrame);

    ECS_SYSTEM(world, EcsSetPlayer, EcsOnSet, EcsPlayer);

    ecs_set(world, EcsWorld, EcsPlayer, {
        .state = EcsPlayerPlay,
        .prev_state = EcsPlayerStop,
        .time_scale = 1,
        .play_pipeline = ecs_get_pipeline(world),
        .stop_pipeline = StopPipeline
    });

    ECS_EXPORT_COMPONENT(EcsPlayer);
}
