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
#include "models/assets.h"
#include "platform/profiler.h"
#include "dev/debug_ui.h"
#include "utils/raylib_helpers.h"
#include "platform/renderer.h"

ZPL_DIAGNOSTIC_PUSH_WARNLEVEL(0)
#include "raylib-nuklear.h"
ZPL_DIAGNOSTIC_POP

#define ARCH_IMPL
#include "platform/arch.h"

#include "gui/tooltip.c"

#include "renderer.c"

void platform_init() {
    platform_create_window("minimal-demo");
    renderer_init();
}

void platform_shutdown() {
    renderer_shutdown();
    CloseWindow();
}

void platform_input() {
    float mouse_z = (GetMouseWheelMove()*0.5f);
    float mouse_modified = target_zoom < 4 ? mouse_z / (zpl_exp(4 - (target_zoom))) : mouse_z;

    if (mouse_z != 0.0f) {
        target_zoom = zpl_clamp(target_zoom + mouse_modified, 0.1f, 11.0f);
    }


    // NOTE(zaklaus): keystate handling
    {
        float x=0.0f, y=0.0f;
        uint8_t use, sprint, ctrl;
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
    platform_resize_window();

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

        debug_draw();
		game_draw_ui();
    }
    EndDrawing();

    if (request_shutdown) {
        CloseWindow();
    }
}
