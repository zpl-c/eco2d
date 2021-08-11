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
static float zoom_overlay_tran = 0.0f;
static bool request_shutdown;

#define CAM_OVERLAY_ZOOM_LEVEL 0.80f

static Camera2D render_camera;

#include "renderer_v0.c"

void platform_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_MAXIMIZED|FLAG_WINDOW_RESIZABLE);
    
    SetTargetFPS(0);
    
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    render_camera.rotation = 0.0f;
    render_camera.zoom = 1.5f;
    
    // NOTE(zaklaus): Paint the screen before we load the game
    // TODO(zaklaus): Render a cool loading screen background maybe? :wink: :wink:
    
    BeginDrawing();
    ClearBackground(GetColor(0x222034));
    
    char const *loading_text = "zpl.eco2d is loading...";
    int text_w = MeasureText(loading_text, 120);
    DrawText(loading_text, GetScreenWidth()-text_w-15, GetScreenHeight()-135, 120, RAYWHITE);
    EndDrawing();
    
    blocks_setup();
    assets_setup();
}

void platform_shutdown() {
    blocks_destroy();
    assets_destroy();
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

void do_entity_fadeinout(uint64_t key, entity_view * data);
void lerp_entity_positions(uint64_t key, entity_view *data);

void platform_render() {
    profile(PROF_ENTITY_LERP) {
        game_world_view_active_entity_map(lerp_entity_positions);
        game_world_view_active_entity_map(do_entity_fadeinout);
    }
    render_camera.zoom = zpl_lerp(render_camera.zoom, target_zoom, 0.18);
    camera_update();
    
    camera game_camera = camera_get();
    render_camera.target = (Vector2){game_camera.x, game_camera.y};
    zoom_overlay_tran = zpl_lerp(zoom_overlay_tran, (target_zoom <= CAM_OVERLAY_ZOOM_LEVEL) ? 1.0f : 0.0f, GetFrameTime()*2.0f);
    
    renderer_draw();
    
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


void lerp_entity_positions(uint64_t key, entity_view *data) {
    (void)key;
    world_view *view = game_world_view_get_active();
    
    if (data->flag == EFLAG_INTERP) {
        
#if 1
        data->x = smooth_val(data->x, data->tx, view->delta_time[data->layer_id]);
        data->y = smooth_val(data->y, data->ty, view->delta_time[data->layer_id]);
        data->heading = smooth_val_spherical(data->heading, data->theading, view->delta_time[data->layer_id]);
#else
        data->x = data->tx;
        data->y = data->ty;
        data->heading = data->theading;
#endif
    }
}

void do_entity_fadeinout(uint64_t key, entity_view * data) {
    (void)key;
    switch (data->tran_effect) {
        case ETRAN_FADEIN: {
            data->tran_time += GetFrameTime();
            
            if (data->tran_time > 1.0f) {
                data->tran_effect = ETRAN_NONE;
                data->tran_time = 1.0f;
            }
        }break;
        
        case ETRAN_FADEOUT: {
            data->tran_time -= GetFrameTime();
            
            if (data->tran_time < 0.0f) {
                data->tran_effect = ETRAN_REMOVE;
                data->tran_time = 0.0f;
            }
        }break;
        
        default: break;
    }
}
