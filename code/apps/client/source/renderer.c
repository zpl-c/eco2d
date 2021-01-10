#include "renderer.h"
#include "raylib.h"
#include "zpl.h"

const zpl_u32 screenWidth = 800;
const zpl_u32 screenHeight = 450;

void gfx_init() {
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
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
