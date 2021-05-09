#include "platform.h"
#include "raylib.h"
#include "raymath.h"
#include "network.h"
#include "game.h"
#include "entity_view.h"
#include "prediction.h"
#include "camera.h"
#include "math.h"

uint16_t screenWidth = 1600;
uint16_t screenHeight = 900;

#define GFX_WORLD_SCALE 20.0f

static Camera2D render_camera;

void DrawTextEco(const char *text, int posX, int posY, int fontSize, Color color, float spacing) {
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        Vector2 position = { (float)posX * GFX_WORLD_SCALE, (float)posY * GFX_WORLD_SCALE };
        
        int defaultFontSize = 10;   // Default Font chars height in pixel
        int new_spacing = spacing == 0.0f ? fontSize/defaultFontSize : spacing;
        
        DrawTextEx(GetFontDefault(), text, position, (float)fontSize * GFX_WORLD_SCALE, (float)new_spacing * GFX_WORLD_SCALE, color);
    }
}

int MeasureTextEco(const char *text, int fontSize, float spacing) {
    Vector2 vec = { 0.0f, 0.0f };
    
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        int defaultFontSize = 10;   // Default Font chars height in pixel
        int new_spacing = spacing == 0.0f ? fontSize/defaultFontSize : spacing;
        
        vec = MeasureTextEx(GetFontDefault(), text, (float)fontSize, (float)new_spacing);
    }
    
    return (int)vec.x;
}
void DrawCircleEco(int centerX, int centerY, float radius, Color color)
{
    DrawCircleV((Vector2){ (float)centerX * GFX_WORLD_SCALE, (float)centerY * GFX_WORLD_SCALE }, radius * GFX_WORLD_SCALE, color);
}
void DrawRectangleEco(int posX, int posY, int width, int height, Color color)
{
    DrawRectangleV((Vector2){ (float)posX * GFX_WORLD_SCALE, (float)posY * GFX_WORLD_SCALE }, (Vector2){ (float)width * GFX_WORLD_SCALE, (float)height * GFX_WORLD_SCALE }, color);
}


void platform_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_MAXIMIZED|FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    render_camera.rotation = 0.0f;
    render_camera.zoom = 4.0f/GFX_WORLD_SCALE;
    
    // NOTE(zaklaus): Paint the screen before we load the game
    // TODO(zaklaus): Render a cool loading screen background maybe? :wink: :wink:
    
    BeginDrawing();
    ClearBackground(GetColor(0x222034));
    
    char const *loading_text = "zpl.eco2d is loading...";
    int text_w = MeasureText(loading_text, 120);
    DrawText(loading_text, GetScreenWidth()-text_w-15, GetScreenHeight()-135, 120, RAYWHITE);
    EndDrawing();
}

void platform_shutdown() {
    CloseWindow();
}
 
uint8_t platform_is_running() {
    return !WindowShouldClose();
}

void platform_input() {
    float mouse_z = (GetMouseWheelMove()*20.0f)/GFX_WORLD_SCALE;
    
    if (mouse_z != 0.0f) {
        render_camera.zoom = zpl_clamp(render_camera.zoom+mouse_z*0.04f, 0.2f/GFX_WORLD_SCALE, 320.0f/GFX_WORLD_SCALE);
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

void DEBUG_draw_entities(uint64_t key, entity_view data);
void DEBUG_draw_ground(uint64_t key, entity_view data);

void lerp_entity_positions(uint64_t key, entity_view data);

void platform_render() {
    game_world_view_active_entity_map(lerp_entity_positions);
    camera_update();

    camera game_camera = camera_get();
    render_camera.target = (Vector2){game_camera.x * GFX_WORLD_SCALE, game_camera.y * GFX_WORLD_SCALE};

    BeginDrawing();
    ClearBackground(GetColor(0x222034));
    BeginMode2D(render_camera);
    game_world_view_active_entity_map(DEBUG_draw_ground);
    game_world_view_active_entity_map(DEBUG_draw_entities);
    EndMode2D();
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
        //DrawText("Connection: ", 5, 5, 52, BLUE);
    }
    
    DrawFPS(0, 20);
}

void DEBUG_draw_ground(uint64_t key, entity_view data) {
    switch (data.kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            int32_t size = view->chunk_size * view->block_size;
            float block_size = view->block_size*0.70f;
            int16_t chunk_size = view->chunk_size;
            int16_t offset = 5;
            float block_spacing = (float)block_size * (size/(float)(chunk_size*block_size));
            float block_offset = size - block_spacing*chunk_size;
            
            double x = data.x * size + offset;
            double y = data.y * size + offset;
            DrawRectangleEco((int)x-offset, (int)y-offset, size+offset, size+offset, BLACK);
            DrawRectangleEco((int)x, (int)y, size-offset, size-offset, LIME);
            
            for (uint16_t i = 0; i < chunk_size*chunk_size; i++) {
                int32_t bx = (float)(i % chunk_size) * block_spacing + (int16_t)x + block_offset;
                int32_t by = (float)(i / chunk_size) * block_spacing + (int16_t)y + block_offset;
                DrawRectangleEco(bx, by, block_size, block_size, GREEN);
            }
            
            DrawTextEco(TextFormat("%.01f %.01f", data.x, data.y), (int16_t)x+15, (int16_t)y+15, 65 , BLACK, 0.0); 
        }break;
        
        default:break;
    }
}

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

void DEBUG_draw_entities(uint64_t key, entity_view data) {
    world_view *view = game_world_view_get_active();
    uint16_t size = 4;
    uint16_t font_size = (uint16_t)lerp(4.0f, 32.0f, 0.5f / GFX_WORLD_SCALE/(float)render_camera.zoom);
    float font_spacing = 1.1f;
    float title_bg_offset = 4;
    float fixed_title_offset = 2;
    
    switch (data.kind) {
        case EKIND_THING: {
            double x = data.x;
            double y = data.y;
            const char *title = TextFormat("Thing %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, BLACK);
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, RAYWHITE, font_spacing); 
            DrawCircleEco(x, y, size, BLUE);
        }break;
        case EKIND_PLAYER: {
            double x = data.x;
            double y = data.y;
            const char *title = TextFormat("Player %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, BLACK);
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, RAYWHITE, font_spacing); 
            DrawCircleEco(x, y, size, RED);
        }break;        
        default:break;
    }
}

void lerp_entity_positions(uint64_t key, entity_view data) {
    world_view *view = game_world_view_get_active();
    
    if (data.flag == EFLAG_INTERP) {
        entity_view *e = entity_view_get(&view->entities, key);
        
#if 1
        e->x = smooth_val(e->x, e->tx, view->delta_time[e->layer_id]);
        e->y = smooth_val(e->y, e->ty, view->delta_time[e->layer_id]);
#else
        e->x = e->tx;
        e->y = e->ty;
#endif
    }
}