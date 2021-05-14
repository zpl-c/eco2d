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
static float target_zoom = 4.0f;
static float zoom_overlay_tran = 0.0f;
static bool request_shutdown;

#define CAM_OVERLAY_ZOOM_LEVEL 0.80f

static Camera2D render_camera;

void platform_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_MAXIMIZED|FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    render_camera.rotation = 0.0f;
    render_camera.zoom = 4.0f;
    
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
        target_zoom = zpl_clamp(target_zoom+mouse_z, 0.3f, 10.0f);
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

void display_conn_status();

void DEBUG_draw_entities(uint64_t key, entity_view * data);
void DEBUG_draw_ground(uint64_t key, entity_view * data);

void lerp_entity_positions(uint64_t key, entity_view * data);
void do_entity_fadeinout(uint64_t key, entity_view * data);

float zpl_lerp(float,float,float);

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

    BeginDrawing();
    profile (PROF_RENDER) {
        ClearBackground(GetColor(0x222034));
        BeginMode2D(render_camera);
        game_world_view_active_entity_map(DEBUG_draw_ground);
        game_world_view_active_entity_map(DEBUG_draw_entities);
        EndMode2D();        
        display_conn_status();
    }
    debug_draw();
    EndDrawing();
    
    if (request_shutdown) {
        CloseWindow();
    }
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

void DEBUG_draw_ground(uint64_t key, entity_view * data) {
    (void)key;
    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            int32_t size = view->chunk_size * WORLD_BLOCK_SIZE;
            int16_t offset = 0;
            
            float x = data->x * size + offset;
            float y = data->y * size + offset;
                             
            DrawRectangleEco(x, y, size-offset, size-offset, ColorAlpha(LIME, data->tran_time));
            
#if 0
            float block_size = view->block_size*0.70f;
            int16_t chunk_size = view->chunk_size;
            float block_spacing = (float)block_size * (size/(float)(chunk_size*block_size));
            float block_offset = size - block_spacing*chunk_size;
            
            for (uint16_t i = 0; i < chunk_size*chunk_size; i++) {
                int32_t bx = (float)(i % chunk_size) * block_spacing + (int16_t)x + block_offset;
                int32_t by = (float)(i / chunk_size) * block_spacing + (int16_t)y + block_offset;
                DrawRectangleEco(bx, by, block_size, block_size, GREEN);
            }
#endif
       
            if (zoom_overlay_tran > 0.02f) {
                DrawRectangleEco(x, y, size-offset, size-offset, ColorAlpha(ColorFromHSV(key*x, 0.13f, 0.89f), data->tran_time*zoom_overlay_tran));
                
                DrawTextEco(TextFormat("%d %d", (int)data->x, (int)data->y), (int16_t)x+15, (int16_t)y+15, 65 , ColorAlpha(BLACK, data->tran_time*zoom_overlay_tran), 0.0); 
                
            }
        }break;
        
        default:break;
    }
}

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

void DEBUG_draw_entities(uint64_t key, entity_view * data) {
    uint16_t size = 4;
    uint16_t font_size = (uint16_t)lerp(4.0f, 32.0f, 0.5f/(float)render_camera.zoom);
    float font_spacing = 1.1f;
    float title_bg_offset = 4;
    float fixed_title_offset = 2;
    
    switch (data->kind) {
        case EKIND_THING: {
            float x = data->x;
            float y = data->y;
#if 0
            const char *title = TextFormat("Thing %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing); 
#endif
            DrawCircleEco(x, y, size, ColorAlpha(BLUE, data->tran_time));
        }break;
        case EKIND_PLAYER: {
            float x = data->x;
            float y = data->y;
            const char *title = TextFormat("Player %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing); 
            DrawCircleEco(x, y, size, ColorAlpha(RED, data->tran_time));
        }break;        
        default:break;
    }
}

void lerp_entity_positions(uint64_t key, entity_view *data) {
    (void)key;
    world_view *view = game_world_view_get_active();
    
    if (data->flag == EFLAG_INTERP) {
        
#if 1
        data->x = smooth_val(data->x, data->tx, view->delta_time[data->layer_id]);
        data->y = smooth_val(data->y, data->ty, view->delta_time[data->layer_id]);
#else
        data->x = data->tx;
        data->y = data->ty;
#endif
    }
}

float platform_frametime() {
    return GetFrameTime();
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

float platform_zoom_get(void) {
    return target_zoom;
}

void platform_request_close(void) {
    request_shutdown = true;
}