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

