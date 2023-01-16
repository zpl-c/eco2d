#include "platform/platform.h"
#include "raylib.h"
#include "raymath.h"
#include "net/network.h"
#include "core/game.h"
#include "world/entity_view.h"
#include "world/prediction.h"
#include "core/camera.h"
#include "math.h"
#include "platform/input.h"
#include "world/blocks.h"
#include "models/assets.h"
#include "platform/profiler.h"
#include "dev/debug_ui.h"
#include "dev/debug_draw.h"
#include "utils/raylib_helpers.h"
#include "platform/renderer.h"

ZPL_DIAGNOSTIC_PUSH_WARNLEVEL(0)
#include "raylib-nuklear.h"
ZPL_DIAGNOSTIC_POP

#define ARCH_IMPL
#include "platform/arch.h"

#include "renderer.c"

// NOTE(zaklaus): add-ins
#include "gui/ui_skin.c"
#include "gui/tooltip.c"
#include "gui/notifications.c"
#include "gui/build_mode.c"
#include "gui/inventory.c"

void platform_init() {
    platform_create_window("eco2d");
    renderer_init();
    
    target_zoom = 2.70f;

	tooltip_register_defaults();

	// room for game-specific tooltips

	tooltip_build_links();

#if 0
	// TEST
	{
		Vector2 mpos = GetMousePosition();
		tooltip_show("ASSET_FURNACE", mpos.x, mpos.y);
	}
#endif

#if 1
	// TEST
	{
		notification_push("test1", "Hello World!");
		notification_push("test2", "Hello World! 2x");
		notification_push("New connection", "A new player has joined the world!");
	}
#endif
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
	tooltip_destroy_all();
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
        uint8_t use, sprint, drop, ctrl, pick;
        if (input_is_down(IN_RIGHT)) x += 1.0f;
        if (input_is_down(IN_LEFT)) x -= 1.0f;
        if (input_is_down(IN_UP)) y += 1.0f;
        if (input_is_down(IN_DOWN)) y -= 1.0f;
        
        use = input_is_pressed(IN_USE);
        sprint = input_is_down(IN_SPRINT);
        ctrl = input_is_down(IN_CTRL);
        drop = input_is_pressed(IN_DROP) || player_inv.drop_item || storage_inv.drop_item;
        
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
            .craft_item = inv->craft_item,
            
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
    platform_resize_window();
    
    profile(PROF_ENTITY_LERP) {
        game_world_view_active_entity_map(lerp_entity_positions);
        game_world_view_active_entity_map(do_entity_fadeinout);
    }
    
    assets_frame();
	set_style(game_ui, THEME_ECO);
    
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

			// goes last
			notification_draw();
			tooltip_draw();
        }
        display_conn_status();
		debug_draw();
		game_draw_ui();
    }
    EndDrawing();
    
    if (request_shutdown) {
        CloseWindow();
    }
}
