#include "debug_ui.h"
#include "raylib.h"
#include "vehicle.h"
#include "camera.h"
#include "world/world.h"
#include "game.h"

#include "modules/components.h"

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
