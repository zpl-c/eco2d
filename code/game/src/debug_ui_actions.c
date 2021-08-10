#include "debug_ui.h"

static inline void
ActExitGame(void) {
    game_request_close();
}

static inline void
ActSpawnCar(void) {
    ecs_entity_t e = vehicle_spawn();
    ecs_entity_t plr = camera_get().ent_id;
    
    Position const* origin = ecs_get(world_ecs(), plr, Position);
    Position * dest = ecs_get_mut(world_ecs(), e, Position, NULL);
    *dest = *origin;
    
    debug_replay_special_action(RPKIND_SPAWN_CAR);
}

// NOTE(zaklaus): Replay system

static inline uint8_t
CondReplayStatusOn(void) {
    return is_recording;
}

static inline uint8_t
CondReplayStatusOff(void) {
    return !is_recording;
}

static inline uint8_t
CondReplayDataPresent(void) {
    return records != NULL && !is_recording;
}

static inline void
ActReplayBegin(void) {
    debug_replay_start();
}

static inline void
ActReplayEnd(void) {
    debug_replay_stop();
}

static inline void
ActReplayRun(void) {
    debug_replay_run();
}

static inline void
ActReplayClear(void) {
    debug_replay_clear();
}


static inline void
ActReplayNew(void) {
    debug_replay_clear();
    zpl_zero_size(replay_filename, sizeof(replay_filename));
}

static inline void
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

static inline void
ActReplaySave(void) {
    if (!replay_filename[0]) {
        ActReplaySaveAs();
    }
    else debug_replay_store();
}

static inline void
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
        zpl_strcpy(replay_filename, path);
        debug_replay_clear();
        debug_replay_load();
    }
}

