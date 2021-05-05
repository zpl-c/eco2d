#include "platform.h"
#include "raylib.h"
#include "network.h"
#include "game.h"
#include "entity_view.h"

const uint16_t screenWidth = 800;
const uint16_t screenHeight = 450;

void platform_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetTargetFPS(60);
}

void platform_shutdown() {
    CloseWindow();
}

uint8_t platform_is_running() {
    return !WindowShouldClose();
}

void display_conn_status();

void DEBUG_draw_entities(uint64_t key, entity_view data);

void platform_render() {
    BeginDrawing();
    ClearBackground(BLACK);
    entity_view_map(DEBUG_draw_entities);
    display_conn_status();
    EndDrawing();
}

void display_conn_status() {
    if (game_is_networked()) {
        if (network_client_is_connected()) {
            DrawText("Connection: online", 5, 5, 12, GREEN);
        } else {
            DrawText("Connection: offline", 5, 5, 12, RED);
        }
    } else {
        DrawText("Connection: single-player", 5, 5, 12, BLUE);
    }
}

void DEBUG_draw_entities(uint64_t key, entity_view data) {
    DrawCircle(data.X, data.Y, 15.0f, RAYWHITE);
}