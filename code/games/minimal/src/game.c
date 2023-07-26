#include "core/game.h"

void game_input() {
	game_core_input();
}

void game_update() {
	game_core_update();
}

void game_render() {
	game_core_render();
}


void game_player_joined(uint64_t ent) {

}

void game_player_departed(uint64_t ent) {

}

void game_player_died(uint64_t ent) {

}

void game_client_receive_code(pkt_send_code data) {

}