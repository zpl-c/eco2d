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

#define GFX_KIND 2
#include "renderers/renderer_bridge.c"

// NOTE(zaklaus): add-ins
#include "gui/build_mode.c"
#include "gui/inventory.c"

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
    // ToggleFullscreen();
    // SetTargetFPS(60.0);

    renderer_init();
}

inline static
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

void platform_shutdown() {
    renderer_shutdown();
    CloseWindow();
}

uint8_t platform_is_running() {
    return !WindowShouldClose();
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

static game_keystate_data last_input_data = {0};
static pkt_send_blockpos last_blockpos_data = {0};

inline static
void platform_input_update_input_frame(game_keystate_data data) {
    float mx = 0, my = 0;
    platform_get_block_realpos(&mx, &my);

    if (mx != last_blockpos_data.mx || my != last_blockpos_data.my){
        last_blockpos_data.mx = mx;
        last_blockpos_data.my = my;
        game_action_send_blockpos(mx, my);
    }

    // NOTE(zaklaus): Test if there are any changes
    if (data.x != last_input_data.x) goto send_data;
    if (data.y != last_input_data.y) goto send_data;
    if (data.use != last_input_data.use) goto send_data;
    if (data.sprint != last_input_data.sprint) goto send_data;
    if (data.ctrl != last_input_data.ctrl) goto send_data;
    if (data.pick != last_input_data.pick) goto send_data;
    if (data.storage_action != last_input_data.storage_action) goto send_data;
    if (data.selected_item != last_input_data.selected_item) goto send_data;
    if (data.drop != last_input_data.drop) goto send_data;
    if (data.swap != last_input_data.swap) goto send_data;
    if (data.swap_from != last_input_data.swap_from) goto send_data;
    if (data.swap_to != last_input_data.swap_to) goto send_data;
    if (data.placement_num != last_input_data.placement_num) goto send_data;
    if (data.deletion_mode != last_input_data.deletion_mode) goto send_data;
    if (zpl_memcompare(data.placements, last_input_data.placements, zpl_size_of(data.placements))) goto send_data;
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
        drop = IsKeyPressed(KEY_G) || player_inv.drop_item || storage_inv.drop_item;

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

        inv_keystate *inv = (inv_is_storage_action) ? &storage_inv : &player_inv;
        inv_keystate *inv2 = (!inv_is_storage_action) ? &storage_inv : &player_inv;

        // NOTE(zaklaus): don't perform picking if we manipulate our inventories
        pick = (inv_is_inside||inv->item_is_held||inv2->item_is_held) ? false : IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        game_keystate_data in_data = {
            .x = x,
            .y = y,
            .mx = mouse_pos.x,
            .my = mouse_pos.y,
            .use = use,
            .sprint = sprint,
            .ctrl = ctrl,
            .pick = pick,

            .drop = drop,
            .storage_action = inv_is_storage_action,
            .selected_item = player_inv.selected_item,
            .storage_selected_item = storage_inv.selected_item,
            .swap = inv->swap,
            .swap_storage = inv_swap_storage,
            .swap_from = inv->swap_from,
            .swap_to = inv->swap_to,

            .deletion_mode = build_is_deletion_mode,
        };

        if (build_submit_placements) {
            in_data.placement_num = build_num_placements;
            zpl_memcopy(in_data.placements, build_placements, build_num_placements*zpl_size_of(item_placement));
        }

        platform_input_update_input_frame(in_data);
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

    // NOTE(zaklaus): switch render modes
    {
        if (IsKeyPressed(KEY_O)) {
            renderer_switch(1-gfx_kind);
        }
    }

    // NOTE(zaklaus): toggle debug drawing
#ifndef ECO2D_PROD
    {
        if (IsKeyPressed(KEY_T)) {
            debug_draw_enable(!debug_draw_state());
        }
    }
#endif
}

void draw_selected_item() {
    camera cam = camera_get();
    entity_view *oe = game_world_view_active_get_entity(cam.ent_id);
    if (oe) {
        // NOTE(zaklaus): sel item
        entity_view *e = game_world_view_active_get_entity(oe->sel_ent);

        if (e && e->kind == EKIND_DEVICE) {
            renderer_draw_single(e->x, e->y, ASSET_BLANK, ColorAlpha(RED, 0.4f));
        }else{
            // NOTE(zaklaus): hover item
            entity_view *e = game_world_view_active_get_entity(oe->pick_ent);

            if (e && e->kind == EKIND_DEVICE) {
                renderer_draw_single(e->x, e->y, ASSET_BLANK, ColorAlpha(RED, 0.1f));
            }
        }
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
            draw_selected_item();
        }
        renderer_debug_draw();
        {
            // NOTE(zaklaus): add-ins
            buildmode_draw();
            inventory_draw();
        }
        display_conn_status();
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
