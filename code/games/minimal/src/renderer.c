static Camera2D render_camera;
static float zoom_overlay_tran = 0.0f;

#define CAM_OVERLAY_ZOOM_LEVEL 0.15f
#define ALPHA(x) ColorAlpha(x, data->tran_time)

float zpl_lerp(float,float,float);
float zpl_to_degrees(float);

void DrawNametag(const char* name, uint64_t key, entity_view *data, float x, float y) {
    float size = 16.f;
    float font_size = lerp(4.0f, 32.0f, 0.5f/(float)render_camera.zoom);
    float font_spacing = 1.1f;
    float title_bg_offset = 4;
    float fixed_title_offset = 8.f;
    float health = (data->hp / data->max_hp);
    const char *title = TextFormat("%s %llu", name, key);
    float title_w = MeasureTextEco(title, font_size, font_spacing);
    DrawRectangleEco(x-title_w/2.f-title_bg_offset/2.f, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
    DrawRectangleEco(x-title_w/2.f-title_bg_offset/2.f, y-size-fixed_title_offset, title_w*health+title_bg_offset, font_size*0.2f, ColorAlpha(RED, data->tran_time));
    DrawTextEco(title, x-title_w/2.f, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing);
}

void DEBUG_draw_overlay(uint64_t key, entity_view * data) {
    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            float size = (float)(view->chunk_size * WORLD_BLOCK_SIZE);
            float offset = 0.0;

            float x = data->x * size + offset;
            float y = data->y * size + offset;

            DrawRectangleEco(x, y, size-offset, size-offset, ColorAlpha(ColorFromHSV((float)data->color, 0.13f, 0.89f), data->tran_time*zoom_overlay_tran*0.75f));
            DrawTextEco(TextFormat("%d %d", (int)data->x, (int)data->y), x+15.0f, y+15.0f, 200 , ColorAlpha(BLACK, data->tran_time*zoom_overlay_tran), 0.0);
        }break;

        default:break;
    }
}

void renderer_draw_entry(uint64_t key, entity_view *data, game_world_render_entry* entry) {
    float size = 16.f;

    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            float size = (float)(view->chunk_size * WORLD_BLOCK_SIZE);
            float offset = 0.0;

            float x = data->x * size + offset;
            float y = data->y * size + offset;

            if (entry == NULL) {
                RenderTexture2D tex = GetChunkTexture(key);
                float scale = (size)/(float)(tex.texture.width);
                tex.texture.width *= (int32_t)scale;
                tex.texture.height *= (int32_t)scale;
                DrawTextureRec(tex.texture, (Rectangle){0, 0, size, -size}, (Vector2){x, y}, ColorAlpha(WHITE, data->tran_time));
            } else {
                DrawTextureRec(GetBlockImage(entry->blk_id), ASSET_SRC_RECT(), (Vector2){entry->x-(WORLD_BLOCK_SIZE/2), entry->y-(WORLD_BLOCK_SIZE/2)}, ColorAlpha(WHITE, data->tran_time));
            }
        }break;
        case EKIND_VEHICLE: {
            float x = data->x;
            float y = data->y;
            float const w = (float)(data->veh_kind == 0 ? 80 : data->veh_kind == 1 ? 120 : 135);
            float const h = 50;
            Color color = data->veh_kind == 0 ? RED : data->veh_kind == 1 ? GREEN : BLUE;
            DrawRectanglePro((Rectangle){x,y,w,h}, (Vector2){w/2.0f,h/2.0f}, zpl_to_degrees(data->heading), ColorAlpha(color, data->tran_time));
        }break;
		case EKIND_SPRITE:
		case EKIND_WEAPON: {
			float x = data->x - 32.f;
			float y = data->y - 32.f;
			DrawTexturePro(GetSpriteTexture2D(assets_find(data->asset)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));
		} break;
        case EKIND_DEVICE:{
            float x = data->x - 32.f;
            float y = data->y - 32.f;
            DrawTexturePro(GetSpriteTexture2D(assets_find(data->asset)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));

            if (data->progress_active) {
                float w = 64.f;
                float h = 8.f;
                float p = data->progress_value;
                float x = data->x - w/2.f;
                float y = data->y - 32.f - h;
                DrawRectangleEco(x, y, w, h, ColorAlpha(BLACK, data->tran_time));
                DrawRectangleEco(x, y, w*p, h, ColorAlpha(GREEN, data->tran_time));
            }
        }break;
        case EKIND_DEMO_NPC: {
            float x = data->x;
            float y = data->y;
            DrawNametag("Demo", key, data, x, y);
            DrawCircleEco(x, y, size, ColorAlpha(BLUE, data->tran_time));
        }break;
        case EKIND_PLAYER: {
            float x = data->x;
            float y = data->y;
            float health = (data->hp / data->max_hp);
            DrawNametag("Player", key, data, x, y);
            DrawCircleEco(x, y, size, ColorAlpha(YELLOW, data->tran_time));

            if (data->has_items && !data->inside_vehicle) {
                float ix = data->x;
                float iy = data->y;
                if (data->items[data->selected_item].quantity > 0) {
                    asset_id it_kind = data->items[data->selected_item].kind;
                    uint32_t qty = data->items[data->selected_item].quantity;
                    DrawTexturePro(GetSpriteTexture2D(assets_find(it_kind)), ASSET_SRC_RECT(), ((Rectangle){ix, iy, 32, 32}), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));
                }
            }
        }break;
        case EKIND_MACRO_BOT: {
            float x = data->x;
            float y = data->y;
            DrawNametag("Bot", key, data, x, y);
        }break;
        case EKIND_ITEM: {
            float x = data->x - 32.f;
            float y = data->y - 32.f;
            DrawTexturePro(GetSpriteTexture2D(assets_find(data->asset)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));

            if (data->quantity > 1) {
                DrawTextEco(zpl_bprintf("%d", data->quantity), x, y, 10, ALPHA(RAYWHITE), 0.0f);
            }

            if (data->durability < 1.0f) {
                DrawRectangleEco(x, y+32, 4, 32, BlendColor(RED, GREEN, data->durability));
                DrawRectangleEco(x, y+32, 4, 32*(1.0f-data->durability), ColorAlpha(BLACK, data->tran_time));
            }
        }break;
        default:break;
    }
}

void renderer_draw(void) {
    render_camera.offset = (Vector2){(float)(screenWidth >> 1), (float)(screenHeight >> 1)};
    render_camera.zoom = zpl_lerp(render_camera.zoom, target_zoom, GetFrameTime()*2.9978f);
    camera_update();

    camera game_camera = camera_get();
    render_camera.target = (Vector2){(float)game_camera.x, (float)game_camera.y};
    zoom_overlay_tran = zpl_lerp(zoom_overlay_tran, (target_zoom <= CAM_OVERLAY_ZOOM_LEVEL) ? 1.0f : 0.0f, GetFrameTime()*2.0f);


    ClearBackground(GetColor(0x222034));
    BeginMode2D(render_camera);

    game_world_view_render_world();

    if (zoom_overlay_tran > 0.02f) {
        game_world_view_active_entity_map(DEBUG_draw_overlay);
    }
    EndMode2D();
}

float renderer_zoom_get(void) {
    return render_camera.zoom;
}

void renderer_init(void) {
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){(float)(screenWidth >> 1), (float)(screenHeight >> 1)};
    render_camera.rotation = 0.0f;
    render_camera.zoom = 2.9f;

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

void renderer_shutdown(void) {
    blocks_destroy();
    assets_destroy();
}

void renderer_debug_draw(void) {

}

void renderer_draw_single(float x, float y, asset_id id, Color color) {
    BeginMode2D(render_camera);

    x -= 32.0f;
    y -= 32.0f;

    DrawTexturePro(GetSpriteTexture2D(assets_find(id)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, color);

    EndMode2D();
}
