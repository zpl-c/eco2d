#include "platform.h"
#include "raylib.h"
#include "network.h"
#include "game.h"
#include "entity_view.h"
#include "camera.h"

const uint16_t screenWidth = 800;
const uint16_t screenHeight = 450;

static Camera2D render_camera;

void platform_init() {
    InitWindow(screenWidth, screenHeight, "eco2d - client");
    SetTargetFPS(60);
    
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    render_camera.rotation = 0.0f;
    render_camera.zoom = 1.0f;
}

void platform_shutdown() {
    CloseWindow();
}

uint8_t platform_is_running() {
    return !WindowShouldClose();
}

void display_conn_status();

void DEBUG_draw_entities(uint64_t key, entity_view data);

void platform_render() {
    camera game_camera = camera_get();
    render_camera.target = (Vector2){game_camera.x, game_camera.y};

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(render_camera);
    DrawRectangleV((Vector2){0,0}, (Vector2){40,40}, RED);
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

void DEBUG_draw_entities(uint64_t key, entity_view data) {
    DrawCircle(data.x, data.y, 15.0f, RAYWHITE);
}