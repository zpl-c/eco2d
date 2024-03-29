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
#include "dev/debug_draw.h" 
#include "dev/debug_ui.h"
#include "utils/raylib_helpers.h"
#include "platform/renderer.h"

ZPL_DIAGNOSTIC_PUSH_WARNLEVEL(0)
#include "raylib-nuklear.h"
ZPL_DIAGNOSTIC_POP

#define ARCH_IMPL
#include "platform/arch.h"


// NOTE(zaklaus): add-ins
#include "gui/ui_skin.c"
#include "gui/tooltip.c"
#include "gui/notifications.c"
#include "gui/spritesheet_viewer.c"

#include "renderer.c"

void platform_init() {
    platform_create_window("horde survival game");
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

void recalc_max_mobs();
extern uint64_t mob_kills;

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
		renderer_debug_draw();

        debug_draw();

#if defined(_DEBUG)
		if (nk_begin(game_ui, "Spritesheet Viewer", nk_rect(460, 100, 800, 600),
		             NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE))
		{
			spritesheet_viewer(game_ui, main_sprite_sheet_nk, main_sprite_sheet.frameSize, main_sprite_sheet.framesPerRow);
			nk_end(game_ui);
		}


		notification_draw();

        if (nk_begin(game_ui, "Debug stuff", nk_rect(400, 10, 220, 140),
                     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE))
        {
            nk_layout_row_dynamic(game_ui, 0, 1);
            if (nk_button_label(game_ui, "max_mobs hack")) {
                mob_kills = 2000;
                recalc_max_mobs();
            }
            if (nk_button_label(game_ui, "big hp")) {
                ecs_entity_t plr = camera_get().ent_id;
                Health *hp = ecs_get_mut(world_ecs(), plr, Health);
                hp->hp = hp->max_hp = 999999;
            }
            nk_end(game_ui);
        }
#endif

		game_draw_ui();
    }
    EndDrawing();

    if (request_shutdown) {
        CloseWindow();
    }
}
