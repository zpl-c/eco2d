#pragma once
#include "platform/system.h"

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#ifdef ARCH_IMPL
EM_JS(int, canvas_get_width, (), {
  return canvas.width;
});

EM_JS(int, canvas_get_height, (), {
  return canvas.height;
});

void UpdateDrawFrame(void) {
    reset_cached_time();
    profile (PROF_MAIN_LOOP) {
        game_input();
        game_update();
        game_render();
    }

    profiler_collate();
}
#else
void UpdateDrawFrame(void);
#endif
#endif

#ifdef ARCH_IMPL
static uint16_t screenWidth = 1024;
static uint16_t screenHeight = 768;
static float target_zoom = 0.6f;
static bool request_shutdown;

static float temp_time = 0.0f;

float get_cached_time(void) {
    return temp_time;
}
void reset_cached_time(void) {
    temp_time = (float)zpl_time_rel();
}

void game_run(void) {
    #if !defined(PLATFORM_WEB)
        while (game_is_running()) {
            reset_cached_time();
            profile (PROF_MAIN_LOOP) {
                game_input();
                game_update();
                game_render();
            }

            profiler_collate();
        }
    #else
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #endif
}
void platform_create_window(const char *title) {
    SetTraceLogLevel(LOG_ERROR);

    #if defined(PLATFORM_WEB)
        screenWidth = (uint16_t)canvas_get_width();
        screenHeight = (uint16_t)canvas_get_height();
    #endif

        InitWindow(screenWidth, screenHeight, title);
        SetWindowState(/*FLAG_WINDOW_UNDECORATED|*/FLAG_WINDOW_MAXIMIZED|FLAG_WINDOW_RESIZABLE|FLAG_MSAA_4X_HINT|FLAG_VSYNC_HINT);

    #if !defined(PLATFORM_WEB)
        screenWidth = (uint16_t)GetScreenWidth();
        screenHeight = (uint16_t)GetScreenHeight();
    #endif
        // ToggleFullscreen();
        // SetTargetFPS(60.0);
}

void platform_resize_window() {
#if !defined(PLATFORM_WEB)
    screenWidth = (uint16_t)GetScreenWidth();
    screenHeight = (uint16_t)GetScreenHeight();
#else
    uint16_t newScreenWidth = (uint16_t)canvas_get_width();
    uint16_t newScreenHeight = (uint16_t)canvas_get_height();
    if (newScreenWidth != screenWidth || newScreenHeight != screenHeight) {
        screenWidth = newScreenWidth;
        screenHeight = newScreenHeight;
        SetWindowSize(screenWidth, screenHeight);
    }
#endif
}

void platform_get_block_realpos(float *x, float *y){
    camera cam = camera_get();
    Vector2 mpos = GetMousePosition();
    entity_view *e = game_world_view_active_get_entity(cam.ent_id);
    if (!e) return;
    float zoom = renderer_zoom_get();
    mpos.x -= screenWidth/2.0f;
    mpos.y -= screenHeight/2.0f;
    cam.x += mpos.x*(1.0f/zoom);
    cam.y += mpos.y*(1.0f/zoom);
    cam.x = ((int32_t)cam.x / (int32_t)(WORLD_BLOCK_SIZE)) * WORLD_BLOCK_SIZE;
    cam.y = ((int32_t)cam.y / (int32_t)(WORLD_BLOCK_SIZE)) * WORLD_BLOCK_SIZE;
    cam.x += WORLD_BLOCK_SIZE/2.0f;
    cam.y += WORLD_BLOCK_SIZE/2.0f;
    if (x) *x = (float)cam.x;
    if (y) *y = (float)cam.y;
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


uint8_t platform_is_running() {
    return !WindowShouldClose();
}

#else
void game_run(void);
#endif
