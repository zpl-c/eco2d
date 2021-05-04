#include "game.h"
#include "platform.h"
#include "world/world.h"
#include "packets/packet.h"
#include "signal_handling.h"

#include "flecs/flecs.h"
#include "flecs/flecs_dash.h"
#include "flecs/flecs_systems_civetweb.h"
#include "flecs/flecs_os_api_stdcpp.h"

void game_init(int8_t play_mode, int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size) {
    
}

int8_t game_is_networked() {
    return 1;
}

void game_shutdown() {
    world_destroy();
}

uint8_t game_is_running() {
    return 1;
}

void game_input() {
    
}

void game_update() {
    world_update();
}

void game_render() {
}

