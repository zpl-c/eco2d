#include "debug_ui.h"
#include "world/blocks.h"

void
ActExitGame(void) {
    game_request_close();
}

void
ActSpawnCar(void) {
    ecs_entity_t e = vehicle_spawn();
    ecs_entity_t plr = camera_get().ent_id;
    
    Position const* origin = ecs_get(world_ecs(), plr, Position);
    Position * dest = ecs_get_mut(world_ecs(), e, Position, NULL);
    *dest = *origin;
    
    debug_replay_special_action(RPKIND_SPAWN_CAR);
}

void
ActSpawnCirclingDriver(void) {
    ecs_entity_t plr = camera_get().ent_id;
    ecs_entity_t ve = vehicle_spawn();
    ecs_entity_t e = entity_spawn(EKIND_DEMO_NPC);
    
    Position const *origin = ecs_get(world_ecs(), plr, Position);
    Position *veh_dest = ecs_get_mut(world_ecs(), ve, Position, NULL);
    Position *dest = ecs_get_mut(world_ecs(), e, Position, NULL);
    *veh_dest = *origin;
    *dest = *origin;
    
    Input *input = ecs_get_mut(world_ecs(), e, Input, NULL);
    input->x = input->y = 1.0f;
    input->use = false;
    
    Vehicle *veh = ecs_get_mut(world_ecs(), ve, Vehicle, NULL);
    veh->seats[0] = e;
    
    ecs_set(world_ecs(), e, IsInVehicle, { .veh = ve });
    
    debug_replay_special_action(RPKIND_SPAWN_CIRCLING_DRIVER);
}

void
ActPlaceIceRink(void) {
    ecs_entity_t plr = camera_get().ent_id;
    uint8_t watr_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);
    Position const *p = ecs_get(world_ecs(), plr, Position);
    float const bs = WORLD_BLOCK_SIZE;
    
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            world_block_lookup l = world_block_from_realpos((p->x - (x*bs)/2.0f), p->y - (y*bs)/2.0f);
            world_chunk_replace_block(world_ecs(), l.chunk_id, l.id, watr_id);
        }
    }
    
    debug_replay_special_action(RPKIND_PLACE_ICE_RINK);
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
    if (zpl_array_count(demo_npcs) >= 10000) return;
    
    for (uint32_t i = 0; i < 1000; i++) {
        uint64_t e = entity_spawn(EKIND_DEMO_NPC);
        ecs_add(world_ecs(), e, EcsDemoNPC);
        Position *pos = ecs_get_mut(world_ecs(), e, Position, NULL);
        pos->x=rand() % world_dim();
        pos->y=rand() % world_dim();        
        
        Velocity *v = ecs_get_mut(world_ecs(), e, Velocity, NULL);
        v->x = (rand()%3-1) * 100;
        v->y = (rand()%3-1) * 100;
        
        zpl_array_append(demo_npcs, e);
    }
}

void
ActDespawnDemoNPCs(void) {
    if (!demo_npcs) return;
    
    for (uint32_t i = 0; i < zpl_array_count(demo_npcs); i++) {
        entity_despawn(demo_npcs[i]);
    }
    
    zpl_array_free(demo_npcs);
    demo_npcs = 0;
}