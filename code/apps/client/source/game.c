#include "game.h"
#include "platform.h"

void game_init() {
    platform_init();
}

void game_shutdown() {
    platform_shutdown();
}

uint8_t game_is_running() {
    return platform_is_running();
}

void game_input() {

}

void game_update() {

}

void game_render() {
    platform_render();
}

