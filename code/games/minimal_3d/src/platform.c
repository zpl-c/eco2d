#include "platform/platform.h"
#include "raylib.h"
#include "raymath.h"
#include "net/network.h"
#include "core/game.h"
#include "world/entity_view.h"
#include "world/prediction.h"
#include "core/camera.h"
#include "math.h"
#include "world/blocks.h"
#include "gen/assets.h"
#include "platform/profiler.h"
#include "debug/debug_ui.h"
#include "utils/raylib_helpers.h"

#if defined(PLATFORM_WEB)
#include <emscripten.h>
EM_JS(int, canvas_get_width, (), {
  return canvas.width;
});

EM_JS(int, canvas_get_height, (), {
  return canvas.height;
});
#endif

static uint16_t screenWidth = 1024;
static uint16_t screenHeight = 768;
static float target_zoom = 0.6f;
static bool request_shutdown;

#include "renderer.c"

bool inv_is_open = false;

void platform_init() {
    SetTraceLogLevel(LOG_ERROR);

#if defined(PLATFORM_WEB)
    screenWidth = (uint16_t)canvas_get_width();
    screenHeight = (uint16_t)canvas_get_height();
#endif

    InitWindow(screenWidth, screenHeight, "eco2d");
    SetWindowState(/*FLAG_WINDOW_UNDECORATED|*/FLAG_WINDOW_MAXIMIZED|FLAG_WINDOW_RESIZABLE|FLAG_MSAA_4X_HINT);

#if !defined(PLATFORM_WEB)
    screenWidth = (uint16_t)GetScreenWidth();
    screenHeight = (uint16_t)GetScreenHeight();
#endif
    renderer_init();
}

void platform_shutdown() {
    renderer_shutdown();
    CloseWindow();
}

uint8_t platform_is_running() {
    return !WindowShouldClose();
}

static game_keystate_data last_input_data = {0};

inline static
void platform_input_update_input_frame(game_keystate_data data) {
    // NOTE(zaklaus): Test if there are any changes
    if (data.x != last_input_data.x) goto send_data;
    if (data.y != last_input_data.y) goto send_data;
    if (data.use != last_input_data.use) goto send_data;
    if (data.sprint != last_input_data.sprint) goto send_data;
    if (data.ctrl != last_input_data.ctrl) goto send_data;
    return;

    send_data:
    last_input_data = data;
    game_action_send_keystate(&data);
}

void platform_input() {
    float mouse_z = (GetMouseWheelMove()*0.5f);

    if (mouse_z != 0.0f) {
        target_zoom = zpl_clamp(target_zoom+mouse_z, 0.1f, 11.0f);
    }

    // NOTE(zaklaus): keystate handling
    {
        float x=0.0f, y=0.0f;
        uint8_t use, sprint, drop, ctrl, pick;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) x += 1.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) x -= 1.0f;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) y += 1.0f;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) y -= 1.0f;

        use = IsKeyPressed(KEY_SPACE);
        sprint = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

        // NOTE(zaklaus): NEW! mouse movement
        Vector2 mouse_pos = GetMousePosition();
        mouse_pos.x /= screenWidth;
        mouse_pos.y /= screenHeight;
        mouse_pos.x -= 0.5f;
        mouse_pos.y -= 0.5f;
        mouse_pos = Vector2Normalize(mouse_pos);

        if (game_get_kind() == GAMEKIND_SINGLE && IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
            x = mouse_pos.x;
            y = -mouse_pos.y;
        }

        game_keystate_data in_data = {
            .x = x,
            .y = y,
            .mx = mouse_pos.x,
            .my = mouse_pos.y,
            .use = use,
            .sprint = sprint,
            .ctrl = ctrl,
        };

        platform_input_update_input_frame(in_data);
    }
}

void platform_render() {
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

    profile(PROF_ENTITY_LERP) {
        game_world_view_active_entity_map(lerp_entity_positions);
        game_world_view_active_entity_map(do_entity_fadeinout);
    }

    assets_frame();

    BeginDrawing();
    {
        profile (PROF_RENDER) {
            renderer_draw();
        }
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
