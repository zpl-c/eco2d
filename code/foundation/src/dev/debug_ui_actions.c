#include "dev/debug_ui.h"
#include "world/blocks.h"
#include "models/items.h"
#include "net/network.h"

#include "models/entity.h"

void
ActExitGame(void) {
    game_request_close();
}

void
ActSpawnCar(void) {
    ecs_entity_t e = vehicle_spawn(EVEH_CAR);
    ecs_entity_t plr = camera_get().ent_id;

    Position const* origin = ecs_get(world_ecs(), plr, Position);
    Position * dest = ecs_get_mut(world_ecs(), e, Position);
    *dest = *origin;
    entity_set_position(e, dest->x, dest->y);

    debug_replay_special_action(RPKIND_SPAWN_CAR);
}

void
ActSpawnItemPrev(void) {
    while (true) {
        --sel_item_id;
        item_id id = item_find_no_proxy(sel_item_id);
        if (sel_item_id > 0 && id != ASSET_INVALID && (item_get_usage(id) != UKIND_PROXY)) {
            break;
        }
    }
}

void
ActSpawnItemNext(void) {
    while (true) {
        ++sel_item_id;
        item_id id = item_find_no_proxy(sel_item_id);
        if (sel_item_id > 0 && id != ASSET_INVALID && (item_get_usage(id) != UKIND_PROXY)) {
            break;
        }
    }
}

void
ActSpawnSelItem(void) {
    ecs_entity_t e = item_spawn(sel_item_id, 32);
    ecs_entity_t plr = camera_get().ent_id;

    Position const* origin = ecs_get(world_ecs(), plr, Position);
    entity_set_position(e, origin->x, origin->y);
}

void
ActSpawnCirclingDriver(void) {
    ecs_entity_t plr = camera_get().ent_id;
    ecs_entity_t ve = vehicle_spawn(EVEH_CAR);
    ecs_entity_t e = entity_spawn(EKIND_DEMO_NPC);

    Position const *origin = ecs_get(world_ecs(), plr, Position);
    Position *veh_dest = ecs_get_mut(world_ecs(), ve, Position);
    Position *dest = ecs_get_mut(world_ecs(), e, Position);
    *veh_dest = *origin;
    *dest = *origin;
    entity_set_position(ve, veh_dest->x, veh_dest->y);
    entity_set_position(e, dest->x, dest->y);

    Input *input = ecs_get_mut(world_ecs(), e, Input);
    zpl_zero_item(input);
    input->x = input->y = 1.0f;

    Vehicle *veh = ecs_get_mut(world_ecs(), ve, Vehicle);
    veh->seats[0] = e;

    ecs_set(world_ecs(), e, IsInVehicle, { .veh = ve });

    debug_replay_special_action(RPKIND_SPAWN_CIRCLING_DRIVER);
}

void
ActPlaceIceRink(void) {
    ecs_entity_t plr = camera_get().ent_id;
    block_id watr_id = blocks_find(ASSET_WATER);
    Position const *p = ecs_get(world_ecs(), plr, Position);
    float const bs = WORLD_BLOCK_SIZE;

    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            world_block_lookup l = world_block_from_realpos((p->x - (x*bs)/2.0f), p->y - (y*bs)/2.0f);
            world_chunk_place_block(l.chunk_id, l.id, watr_id);
        }
    }

    debug_replay_special_action(RPKIND_PLACE_ICE_RINK);
}

void
ActEraseWorldChanges(void) {
    ecs_entity_t plr = camera_get().ent_id;
    Position const *p = ecs_get(world_ecs(), plr, Position);
    float const bs = WORLD_BLOCK_SIZE;

    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            world_chunk_destroy_block((p->x - (x*bs)/2.0f), (p->y - (y*bs)/2.0f), true);
        }
    }

    debug_replay_special_action(RPKIND_PLACE_ERASE_CHANGES);
}

// NOTE(zaklaus): Replay system

uint8_t
CondReplayStatusOn(void) {
    return is_recording && !is_playing;
}

uint8_t
CondReplayStatusOff(void) {
    return !is_recording && !is_playing;
}

uint8_t
CondReplayDataPresentAndNotPlaying(void) {
    return records != NULL && !is_recording && !is_playing;
}

uint8_t
CondReplayIsPlaying(void) {
    return records != NULL && !is_recording && is_playing;
}

uint8_t
CondReplayIsNotPlaying(void) {
    return !is_recording && !is_playing;
}

uint8_t
CondReplayIsNotPlayingOrRecordsNotClear(void) {
    return records != NULL && !is_recording && !is_playing;
}

void
ActReplayBegin(void) {
    debug_replay_start();
}

void
ActReplayEnd(void) {
    debug_replay_stop();
}

void
ActReplayRun(void) {
    debug_replay_run();
}

void
ActReplayClear(void) {
    debug_replay_clear();
}


void
ActReplayNew(void) {
    debug_replay_clear();
    zpl_zero_size(replay_filename, sizeof(replay_filename));
}

void
ActReplaySaveAs(void) {
    if (!records) return;
    char const *workdir = GetWorkingDirectory();

    sfd_Options sfd = {
        .title = "Save Macro",
        .path = "art",
        .filter_name = "eco2d Macro",
        .filter = "*.dem",
    };

    char const *path = sfd_save_dialog(&sfd);
    ChangeDirectory(workdir);

    if (path) {
        zpl_strcpy(replay_filename, zpl_bprintf("%s.dem", path));
        debug_replay_store();
    }

}

void
ActReplaySave(void) {
    if (!replay_filename[0]) {
        ActReplaySaveAs();
    }
    else debug_replay_store();
}

void
ActReplayLoad(void) {
    char const *workdir = GetWorkingDirectory();

    sfd_Options sfd = {
        .title = "Load Macro",
        .path = "art",
        .filter_name = "eco2d Macro",
        .filter = "*.dem",
    };

    char const *path = sfd_open_dialog(&sfd);
    ChangeDirectory(workdir);

    if (path) {
        zpl_zero_size(replay_filename, sizeof(replay_filename));
        zpl_strcpy(replay_filename, path);
        debug_replay_clear();
        debug_replay_load();
    }
}

// NOTE(zaklaus): Demo NPCs
static ecs_entity_t *demo_npcs = NULL;

void
ActSpawnDemoNPCs(void) {
    if (!demo_npcs) zpl_array_init(demo_npcs, zpl_heap());
    if (zpl_array_count(demo_npcs) >= 100000) return;

    for (uint32_t i = 0; i < 1000; i++) {
        uint64_t e = entity_spawn_id(ASSET_CREATURE);
        Position *pos = ecs_get_mut(world_ecs(), e, Position);
        pos->x=(float)(rand() % world_dim());
        pos->y=(float)(rand() % world_dim());

        Velocity *v = ecs_get_mut(world_ecs(), e, Velocity);
        v->x = (float)((rand()%3-1) * 10);
        v->y = (float)((rand()%3-1) * 10);

        zpl_array_append(demo_npcs, e);
    }
}

void
ActDespawnDemoNPCs(void) {
    if (!demo_npcs) return;

    entity_batch_despawn(demo_npcs, zpl_array_count(demo_npcs));

    zpl_array_free(demo_npcs);
    demo_npcs = 0;
}

// NOTE(zaklaus): world simulation controls
#define WORLDSIM_STEPPING 0.01f
static float sim_step_size = 0.1f;

void
ActWorldToggleSim(void) {
    if (world_is_paused()) {
        world_resume();
    } else {
        world_pause();
    }
}

void
ActWorldIncrementSimStepSize(void) {
    sim_step_size += WORLDSIM_STEPPING;
}

void
ActWorldDecrementSimStepSize(void) {
    if (sim_step_size > WORLDSIM_STEPPING)
        sim_step_size -= WORLDSIM_STEPPING;
}

void
ActWorldStep(void) {
    world_step(sim_step_size);
}

uint8_t
CondIsWorldPaused(void) {
    return world_is_paused();
}

uint8_t
CondIsWorldRunning(void) {
    return !world_is_paused();
}

// NOTE(zaklaus): connection metrics

uint8_t
CondClientConnected(void) {
    return network_client_is_connected();
}

uint8_t
CondClientDisconnected(void) {
    return !network_client_is_connected();
}
