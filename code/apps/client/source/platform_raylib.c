#include "platform.h"
#include "raylib.h"
#include "network.h"
#include "game.h"
#include "entity_view.h"
#include "camera.h"

const uint16_t screenWidth = 1600;
const uint16_t screenHeight = 900;

static Camera2D render_camera;

void DrawTextEco(const char *text, int posX, int posY, int fontSize, Color color, float spacing) {
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        Vector2 position = { (float)posX, (float)posY };
        
        int defaultFontSize = 10;   // Default Font chars height in pixel
        int new_spacing = spacing == 0.0f ? fontSize/defaultFontSize : spacing;
        
        DrawTextEx(GetFontDefault(), text, position, (float)fontSize, (float)new_spacing, color);
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


void platform_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetTargetFPS(60);
    
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    render_camera.rotation = 0.0f;
    render_camera.zoom = 8.0f;
}

void platform_shutdown() {
    CloseWindow();
}
 
uint8_t platform_is_running() {
    return !WindowShouldClose();
}

void platform_input() {
    float mouse_z = GetMouseWheelMove();
    
    if (mouse_z != 0.0f) {
        render_camera.zoom = zpl_clamp(render_camera.zoom+mouse_z*0.04f, 0.01f, 16.0f);
    }
    
    // NOTE(zaklaus): keystate handling
    {
        double x=0.0, y=0.0;
        uint8_t use, sprint;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) x += 1.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) x -= 1.0f;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) y -= 1.0f;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) y += 1.0f;
        
        use = IsKeyPressed(KEY_SPACE);
        sprint = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        
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

void platform_render() {
    camera game_camera = camera_get();
    render_camera.target = (Vector2){game_camera.x, game_camera.y};

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(render_camera);
    entity_view_map(&game_world_view_get_active()->entities, DEBUG_draw_ground);
    entity_view_map(&game_world_view_get_active()->entities, DEBUG_draw_entities);
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
    }
}

void DEBUG_draw_ground(uint64_t key, entity_view data) {
    world_view *view = game_world_view_get_active();
    
    int32_t x = data.x * view->chunk_size * view->block_size;
    int32_t y = data.y * view->chunk_size * view->block_size;
                    
    int32_t size = view->chunk_size * view->block_size;
    int32_t half_size = size/2;
    int32_t half_block_size = view->block_size/2;
    int16_t offset = 10;
    int16_t block_offset = 1;
  
    switch (data.kind) {
        case EKIND_CHUNK: {
            DrawRectangle(x+offset-half_size, y+offset-half_size, size-offset, size-offset, LIME);
            for (uint16_t i = 0; i < view->chunk_size*view->chunk_size; i++) {
                int32_t bx = i % view->block_size * view->block_size + x - half_size + offset;
                int32_t by = i / view->block_size * view->block_size + y - half_size + offset;
                DrawRectangle(bx+block_offset-half_block_size,
                                  by+block_offset-half_block_size,
                                  view->block_size-block_offset,
                                  view->block_size-block_offset,
                                  GREEN);
            }
            
            DrawText(TextFormat("%.01f %.01f", data.x, data.y), x-half_size+5, y-half_size+5, 65, BLACK); 
        }break;
        
        default:break;
    }
}

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

void DEBUG_draw_entities(uint64_t key, entity_view data) {
    world_view *view = game_world_view_get_active();
    uint16_t size = 4;
    uint16_t font_size = (uint16_t)lerp(4, 32, 0.5f/render_camera.zoom);
    float font_spacing = 1.1f;
    
    switch (data.kind) {
        case EKIND_PLAYER: {
            const char *title = TextFormat("Player %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawTextEco(title, data.x-title_w/2, data.y-size-font_size, font_size, BLACK, font_spacing); 
            DrawCircle(data.x, data.y, size, RED);
        }break;
        
        default:break;
    }
}