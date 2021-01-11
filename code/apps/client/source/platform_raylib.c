#include "platform.h"
#include "raylib.h"

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

void platform_render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("NOBODY EXPECTS SPANISH INQUISITION!", 190, 200, 20, RED);
    EndDrawing();
}
