#include "platform.h"
#include "raylib.h"
#include "raymath.h"
#include "network.h"
#include "game.h"
#include "entity_view.h"
#include "prediction.h"
#include "camera.h"
#include "math.h"
#include "world/blocks.h"
#include "assets.h"
#include "profiler.h"
#include "debug_ui.h"
#include "utils/raylib_helpers.h"

static uint16_t screenWidth = 1600;
static uint16_t screenHeight = 900;
static float target_zoom = 1.5f;
static bool request_shutdown;

#include "renderer_v0.c"

void platform_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_MAXIMIZED|FLAG_WINDOW_RESIZABLE);
    
    SetTargetFPS(0);
    
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    
    renderer_init();
}

void platform_shutdown() {
    renderer_shutdown();
    CloseWindow();
}

uint8_t platform_is_running() {
    return !WindowShouldClose();
}

void platform_input() {
    float mouse_z = (GetMouseWheelMove()*0.5f);
    
    if (mouse_z != 0.0f) {
        target_zoom = zpl_clamp(target_zoom+mouse_z, 0.1f, 10.0f);
    }
    
    // NOTE(zaklaus): keystate handling
    {
        float x=0.0f, y=0.0f;
        uint8_t use, sprint;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) x += 1.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) x -= 1.0f;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) y -= 1.0f;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) y += 1.0f;
        
        use = IsKeyPressed(KEY_SPACE);
        sprint = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        
        // NOTE(zaklaus): NEW! mouse movement
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            Vector2 mouse_pos = GetMousePosition();
            mouse_pos.x /= screenWidth;
            mouse_pos.y /= screenHeight;
            mouse_pos.x -= 0.5f;
            mouse_pos.y -= 0.5f;
            mouse_pos = Vector2Normalize(mouse_pos);
            x = mouse_pos.x;
            y = mouse_pos.y;
        }
        
        game_action_send_keystate(x, y, use, sprint);
    }
    
    // NOTE(zaklaus): cycle through viewers
    {
        if (IsKeyPressed(KEY_Q)) {
            game_world_view_cycle_active(-1);
        }
        else if (IsKeyPressed(KEY_E)) {
            game_world_view_cycle_active(1);
        }
    }
}

void platform_render() {
    profile(PROF_ENTITY_LERP) {
        game_world_view_active_entity_map(lerp_entity_positions);
        game_world_view_active_entity_map(do_entity_fadeinout);
    }
    
    BeginDrawing();
    {
        profile (PROF_RENDER) {
            renderer_draw();
        }
        debug_draw();
    }
    EndDrawing();
    
    if (request_shutdown) {
        CloseWindow();
    }
}

float platform_frametime() {
    return GetFrameTime();
}

float platform_zoom_get(void) {
    return target_zoom;
}

void platform_request_close(void) {
    request_shutdown = true;
}
