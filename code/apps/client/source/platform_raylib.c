#include "platform.h"
#include "raylib.h"
#include "game.h"

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

void platform_render() {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("NOBODY EXPECTS SPANISH INQUISITION!", 190, 200, 20, RED);
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