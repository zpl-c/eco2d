#include "system.h"
#include "renderer.h"
#include "raylib.h"

const uint16_t screenWidth = 800;
const uint16_t screenHeight = 450;

void gfx_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetTargetFPS(60);
}

void gfx_shutdown() {
    CloseWindow();
}

void gfx_render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("NOBODY EXPECTS SPANISH INQUISITION!", 190, 200, 20, RED);
    EndDrawing();
}
