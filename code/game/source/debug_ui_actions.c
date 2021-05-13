#include "debug_ui.h"
#include "raylib.h"
#include "game.h"

static inline void
ActExitGame(void) {
    game_request_close();
}