#include "debug_ui.h"
#include "raylib.h"
#include "vehicle.h"
#include "game.h"

static inline void
ActExitGame(void) {
    game_request_close();
}

static inline void
ActSpawnCar(void) {
    vehicle_spawn();
}