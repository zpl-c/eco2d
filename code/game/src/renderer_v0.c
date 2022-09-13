static Camera2D render_camera;
static float zoom_overlay_tran = 0.0f;

#define CAM_OVERLAY_ZOOM_LEVEL 0.80f
#define ALPHA(x) ColorAlpha(x, data->tran_time)

float zpl_lerp(float,float,float);
float zpl_to_degrees(float);

void DEBUG_draw_ground(uint64_t key, entity_view * data) {
    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            float size = (float)(view->chunk_size * WORLD_BLOCK_SIZE);
            float offset = 0.0;

            float x = data->x * size + offset;
            float y = data->y * size + offset;

            RenderTexture2D tex = GetChunkTexture(key);
            float scale = (size)/(float)(tex.texture.width);
            tex.texture.width *= (int32_t)scale;
            tex.texture.height *= (int32_t)scale;
            DrawTextureRec(tex.texture, (Rectangle){0, 0, size, -size}, (Vector2){x, y}, ColorAlpha(WHITE, data->tran_time));

            for (size_t ty = 0; ty < view->chunk_size; ty++) {
                for (size_t tx = 0; tx < view->chunk_size; tx++) {
                    block_id blk_id = data->outer_blocks[(ty*view->chunk_size)+tx];
                    if (blk_id != 0) {
                        DrawTextureRec(GetBlockImage(blk_id), ASSET_SRC_RECT(), (Vector2){x+tx*WORLD_BLOCK_SIZE, y+ty*WORLD_BLOCK_SIZE}, ColorAlpha(WHITE, data->tran_time));
                    }
                }
            }
        }break;

        default:break;
    }
}

void DEBUG_draw_overlay(uint64_t key, entity_view * data) {
    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            float size = (float)(view->chunk_size * WORLD_BLOCK_SIZE);
            float offset = 0.0;

            float x = data->x * size + offset;
            float y = data->y * size + offset;

            if (zoom_overlay_tran > 0.02f) {
                DrawRectangleEco(x, y, size-offset, size-offset, ColorAlpha(ColorFromHSV((float)data->color, 0.13f, 0.89f), data->tran_time*zoom_overlay_tran*0.75f));
                DrawTextEco(TextFormat("%d %d", (int)data->x, (int)data->y), x+15.0f, y+15.0f, 200 , ColorAlpha(BLACK, data->tran_time*zoom_overlay_tran), 0.0);
            }
        }break;

        default:break;
    }
}

extern bool inv_is_open;

void DEBUG_draw_entities(uint64_t key, entity_view * data) {
    float size = 16.f;
    float font_size = lerp(4.0f, 32.0f, 0.5f/(float)render_camera.zoom);
    float font_spacing = 1.1f;
    float title_bg_offset = 4;
    float fixed_title_offset = 8.f;

    switch (data->kind) {
        case EKIND_DEMO_NPC: {
            float x = data->x;
            float y = data->y;
            DrawCircleEco(x, y, size, ColorAlpha(BLUE, data->tran_time));
        }break;
        case EKIND_PLAYER: {
            float x = data->x;
            float y = data->y;
            float health = (data->hp / data->max_hp);
            const char *title = TextFormat("Player %d", key);
            float title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2.f-title_bg_offset/2.f, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
            DrawRectangleEco(x-title_w/2.f-title_bg_offset/2.f, y-size-fixed_title_offset, title_w*health+title_bg_offset, font_size*0.2f, ColorAlpha(RED, data->tran_time));
            DrawTextEco(title, x-title_w/2.f, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing);
            DrawCircleEco(x, y, size, ColorAlpha(YELLOW, data->tran_time));

            if (data->has_items && !data->inside_vehicle) {
                float ix = data->x;
                float iy = data->y;
                if (data->items[data->selected_item].quantity > 0) {
                    asset_id it_kind = data->items[data->selected_item].kind;
                    uint32_t qty = data->items[data->selected_item].quantity;
                    DrawTexturePro(GetSpriteTexture2D(assets_find(it_kind)), ASSET_SRC_RECT(), ((Rectangle){ix, iy, 32, 32}), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));

                    if (!inv_is_open)
                        DrawTextEco(zpl_bprintf("%d", qty), x+24, y+24, 8, RAYWHITE, 0.0f);
                }
            }
        }break;
        case EKIND_MACRO_BOT: {
            float x = data->x;
            float y = data->y;
            const char *title = TextFormat("Bot %d", key);
            float title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2.f-title_bg_offset/2.f, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(GRAY, data->tran_time));
            DrawTextEco(title, x-title_w/2.f, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(BLACK, data->tran_time), font_spacing);
            DrawCircleEco(x, y, size, ColorAlpha(PURPLE, data->tran_time));
        }break;
        case EKIND_ITEM: {
            float x = data->x - 32.f;
            float y = data->y - 32.f;
            DrawTexturePro(GetSpriteTexture2D(assets_find(data->asset)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));
            DrawTextEco(zpl_bprintf("%d", data->quantity), x, y, 10, ALPHA(RAYWHITE), 0.0f);
        }break;
        default:break;
    }
}

void DEBUG_draw_entities_low(uint64_t key, entity_view * data) {
    (void)key;

    switch (data->kind) {
        case EKIND_VEHICLE: {
            float x = data->x;
            float y = data->y;
            float const w = 80;
            float const h = 50;
            DrawRectanglePro((Rectangle){x,y,w,h}, (Vector2){w/2.0f,h/2.0f}, zpl_to_degrees(data->heading), ColorAlpha(RED, data->tran_time));
        }break;
        case EKIND_DEVICE:{
            float x = data->x - 32.f;
            float y = data->y - 32.f;
            DrawTexturePro(GetSpriteTexture2D(assets_find(data->asset)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));
        }break;
        default:break;
    }
}

void renderer_draw_v0(void) {
    render_camera.offset = (Vector2){(float)(screenWidth >> 1), (float)(screenHeight >> 1)};
    render_camera.zoom = zpl_lerp(render_camera.zoom, target_zoom, GetFrameTime()*2.9978f);
    camera_update();

    camera game_camera = camera_get();
    render_camera.target = (Vector2){(float)game_camera.x, (float)game_camera.y};
    zoom_overlay_tran = zpl_lerp(zoom_overlay_tran, (target_zoom <= CAM_OVERLAY_ZOOM_LEVEL) ? 1.0f : 0.0f, GetFrameTime()*2.0f);


    ClearBackground(GetColor(0x222034));
    BeginMode2D(render_camera);
    game_world_view_active_entity_map(DEBUG_draw_ground);
    game_world_view_active_entity_map(DEBUG_draw_entities_low);
    game_world_view_active_entity_map(DEBUG_draw_entities);
    game_world_view_active_entity_map(DEBUG_draw_overlay);
    EndMode2D();
}

float renderer_zoom_get_v0(void) {
    return render_camera.zoom;
}

void renderer_init_v0(void) {
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){(float)(screenWidth >> 1), (float)(screenHeight >> 1)};
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

void renderer_shutdown_v0(void) {
    blocks_destroy();
    assets_destroy();
}

void renderer_debug_draw_v0(void) {
    BeginMode2D(render_camera);
    debug_draw_queue *que = debug_draw_samples();

    for (size_t i = 0; i < que->num_entries; i += 1) {
        debug_draw_entry *e = &que->entries[i];
        Color color = GetColor(e->color);

        switch (e->kind) {
            case DDRAW_LINE: {
                float x = e->a.x;
                float y = e->a.y;
                float x2 = e->b.x;
                float y2 = e->b.y;
                DrawLineV((Vector2){x, y}, (Vector2){x2, y2}, color);
            }break;

            case DDRAW_CIRCLE:{
                float x = e->a.x;
                float y = e->a.y;
                DrawCircleLinesEco(x, y, e->radius, color);
            }break;

            case DDRAW_RECT:{
                float x = e->bmin.x;
                float y = e->bmin.y;
                float w = e->bmax.x - e->bmin.x;
                float h = e->bmax.y - e->bmin.y;
                DrawRectangleLinesEco(x, y, w, h, color);
            }break;

            default: {

            }break;
        }
    }

    EndMode2D();
}

void renderer_draw_single_v0(float x, float y, asset_id id, Color color) {
    BeginMode2D(render_camera);

    x -= 32.0f;
    y -= 32.0f;

    DrawTexturePro(GetSpriteTexture2D(assets_find(id)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, color);

    EndMode2D();
}
