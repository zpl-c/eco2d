#include "game.h"
#include "renderer.h"

void game_init() {
    gfx_init();
}

void game_shutdown() {
    gfx_shutdown();
}

uint8_t game_is_running() {
    return gfx_is_running();
}

void game_input() {

}

void game_update() {

}

void game_render() {
    gfx_render();
}

