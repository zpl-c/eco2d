static Camera3D render_camera;
static float zoom_overlay_tran = 0.0f;
static float cam_zoom = 1.5f;

#define CAM_OVERLAY_ZOOM_LEVEL 0.15f
#define ALPHA(x) ColorAlpha(x, data->tran_time)

float zpl_lerp(float,float,float);
float zpl_to_degrees(float);

void DrawNametag(const char* name, uint64_t key, entity_view *data, float x, float y) {
    float size = 72.f;
    float font_size = lerp(12.0f, 72.0f, 0.5f/1.0f);//(float)render_camera.zoom);
    float font_spacing = 1.1f;
    float title_bg_offset = 4;
    float fixed_title_offset = 8.f;
    float health = (data->hp / data->max_hp);
    const char *title = TextFormat("%s %llu", name, key);
    float title_w = MeasureTextEco(title, font_size, font_spacing);
    Draw3DRectangle(render_camera, x-title_w/2.f-title_bg_offset/2.f, y, size+font_size+fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
    Draw3DRectangle(render_camera, x-title_w/2.f-title_bg_offset/2.f, y, size+fixed_title_offset, title_w*health+title_bg_offset, font_size*0.2f, ColorAlpha(RED, data->tran_time));
    DrawTextEco(title, x-title_w/2.f, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing);
    DrawText3D(GetFontDefault(), title, (Vector3){x-title_w/2.f, 64.f, y}, 24.f, 12.0f, 1.3f, true, WHITE);
}

void DEBUG_draw_ground(uint64_t key, entity_view * data) {
    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            float size = (float)(view->chunk_size * WORLD_BLOCK_SIZE);
            float half_size = size / 2.0f;
            float half_block_size = (float)(WORLD_BLOCK_SIZE >> 1);
            float offset = 0.0;

            float x = data->x * size + offset;
            float y = data->y * size + offset;

            RenderTexture2D tex = GetChunkTexture(key);
            float scale = (size)/(float)(tex.texture.width);
            tex.texture.width *= (int32_t)scale;
            tex.texture.height *= (int32_t)scale;
            // DrawTextureRec(tex.texture, (Rectangle){0, 0, size, -size}, (Vector2){x, y}, ColorAlpha(WHITE, data->tran_time));
            DrawCubeTexture(tex.texture, (Vector3){x+half_size, 0.0f, y+half_size}, size, 0.01f, size, WHITE);
//            DrawCubeWires((Vector3){x, 0.f, y}, WORLD_BLOCK_SIZE*view->chunk_size, 66, WORLD_BLOCK_SIZE*view->chunk_size, BLUE);

            for (size_t ty = 0; ty < view->chunk_size; ty++) {
                for (size_t tx = 0; tx < view->chunk_size; tx++) {
                    block_id blk_id = data->outer_blocks[(ty*view->chunk_size)+tx];
                    if (blk_id != 0) {
                        // DrawTextureRec(GetBlockImage(blk_id), ASSET_SRC_RECT(), (Vector2){x+tx*WORLD_BLOCK_SIZE, y+ty*WORLD_BLOCK_SIZE}, ColorAlpha(WHITE, data->tran_time));
                        DrawCubeTexture(GetBlockImage(blk_id), (Vector3){x + tx*WORLD_BLOCK_SIZE+half_block_size, 32.0f, y + ty*WORLD_BLOCK_SIZE+half_block_size}, 64, 0.01f, 64, WHITE);
                    }
                }
            }

            for (int by = 0; by < 16; by++) {
                for (int bx = 0; bx < 16; bx++) {
                    switch (blocks_get_flags(data->blocks[by*16+bx])) {
                        case BLOCK_FLAG_COLLISION:{
                            DrawCubeWires((Vector3){x + bx*WORLD_BLOCK_SIZE+half_block_size, 34.f, y + by*WORLD_BLOCK_SIZE+half_block_size}, 64, 66, 64, BLUE);
                        }break;
                        case BLOCK_FLAG_HAZARD:{
                            DrawCubeWires((Vector3){x + bx*WORLD_BLOCK_SIZE+half_block_size, 16.667f, y + by*WORLD_BLOCK_SIZE+half_block_size}, 64, 33, 64, RED);
                        }break;
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

            DrawRectangleEco(x, y, size-offset, size-offset, ColorAlpha(ColorFromHSV((float)data->color, 0.13f, 0.89f), data->tran_time*zoom_overlay_tran*0.75f));
            DrawTextEco(TextFormat("%d %d", (int)data->x, (int)data->y), x+15.0f, y+15.0f, 200 , ColorAlpha(BLACK, data->tran_time*zoom_overlay_tran), 0.0);
        }break;

        default:break;
    }
}

extern bool inv_is_open;

void DEBUG_draw_entities(uint64_t key, entity_view * data) {
    world_view *view = game_world_view_get_active();
    float size = 16.f;
    uint16_t ground_offset = 30;
    float bl_size = (float)(view->chunk_size * WORLD_BLOCK_SIZE);
    float half_size = bl_size / 2.0f;
    float half_block_size = (float)(WORLD_BLOCK_SIZE >> 1);
    Texture2D sprite_tex = GetSpriteTexture2D(assets_find(data->asset));

    switch (data->kind) {
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
            // DrawCircleEco(x, y, size, ColorAlpha(YELLOW, data->tran_time));
            DrawSphere((Vector3){x,ground_offset,y}, size, ColorAlpha(YELLOW, data->tran_time));

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
            DrawNametag("Bot", key, data, x, y);
        }break;
        case EKIND_ITEM: {
            float x = data->x - 32.f;
            float y = data->y - 32.f;
            // DrawTexturePro(GetSpriteTexture2D(assets_find(data->asset)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));
            // DrawCubeTexture(GetSpriteTexture2D(assets_find(data->asset)), (Vector3){x + half_block_size, 32.0f, y + half_block_size}, 64, 0.01f, 64, WHITE);
            Draw3DBillboard(render_camera, sprite_tex, (Vector3){x + half_block_size, 32.0f, y + half_block_size}, 64.0f, WHITE);

            if (data->quantity > 1) {
                DrawTextEco(zpl_bprintf("%d", data->quantity), x, y, 10, ALPHA(RAYWHITE), 0.0f);
            }

            if (data->durability < 1.0f) {
                DrawRectangleEco(x, y+32, 4, 32, BlendColor(RED, GREEN, data->durability));
                DrawRectangleEco(x, y+32, 4, 32*(1.0f-data->durability), ColorAlpha(BLACK, data->tran_time));
            }
        }break;
        case EKIND_DEVICE: {
            float x = data->x - 32.f;
            float y = data->y - 32.f;
            Draw3DBillboard(render_camera, sprite_tex, (Vector3){x + half_block_size, 32.0f, y + half_block_size}, 64.0f, WHITE);

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

void DEBUG_draw_entities_low(uint64_t key, entity_view * data) {
    (void)key;

    switch (data->kind) {
        case EKIND_VEHICLE: {
            float x = data->x;
            float y = data->y;
            float const w = (float)(data->veh_kind == 0 ? 80 : data->veh_kind == 1 ? 120 : 135);
            float const h = 50;
            Color color = data->veh_kind == 0 ? RED : data->veh_kind == 1 ? GREEN : BLUE;
            DrawRectanglePro((Rectangle){x,y,w,h}, (Vector2){w/2.0f,h/2.0f}, zpl_to_degrees(data->heading), ColorAlpha(color, data->tran_time));
        }break;
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
        default:break;
    }
}

void renderer_draw(void) {
    cam_zoom = zpl_min(zpl_lerp(cam_zoom, target_zoom, GetFrameTime()*2.18f), 9.98f);
    camera_update();

    camera game_camera = camera_get();
#if 1
    render_camera.position = (Vector3){(float)game_camera.x, 260.0f*(10.0f-cam_zoom)+30.0f, (float)game_camera.y-50.0f*(10.0f-cam_zoom/2.0f)};
    render_camera.target = (Vector3){(float)game_camera.x, 30.0f, (float)game_camera.y};
#else
    UpdateCamera(&render_camera);
#endif

    // render_camera.offset = (Vector2){(float)(screenWidth >> 1), (float)(screenHeight >> 1)};
    // render_camera.zoom = zpl_lerp(render_camera.zoom, target_zoom, GetFrameTime()*2.9978f);
    // camera_update();

    // camera game_camera = camera_get();
    // render_camera.target = (Vector2){(float)game_camera.x, (float)game_camera.y};
    // zoom_overlay_tran = zpl_lerp(zoom_overlay_tran, (target_zoom <= CAM_OVERLAY_ZOOM_LEVEL) ? 1.0f : 0.0f, GetFrameTime()*2.0f);


    ClearBackground(GetColor(0x222034));
    BeginMode3D(render_camera);
    game_world_view_active_entity_map(DEBUG_draw_ground);
    game_world_view_active_entity_map(DEBUG_draw_entities_low);
    game_world_view_active_entity_map(DEBUG_draw_entities);

    if (zoom_overlay_tran > 0.02f) {
        game_world_view_active_entity_map(DEBUG_draw_overlay);
    }
    EndMode3D();
}

float renderer_zoom_get(void) {
    return 1.0f;//render_camera.zoom;
}

void renderer_init(void) {
    // render_camera.target = (Vector2){0.0f,0.0f};
    // render_camera.offset = (Vector2){(float)(screenWidth >> 1), (float)(screenHeight >> 1)};
    // render_camera.rotation = 0.0f;
    // render_camera.zoom = 2.9f;

    render_camera.up = (Vector3){0.0f,1.0f,0.0f};
    render_camera.fovy = 45.f;
    render_camera.projection = CAMERA_PERSPECTIVE;

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
    BeginMode3D(render_camera);
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

    EndMode3D();
}

void renderer_draw_single(float x, float y, asset_id id, Color color) {
    BeginMode3D(render_camera);

    x -= 32.0f;
    y -= 32.0f;

    DrawTexturePro(GetSpriteTexture2D(assets_find(id)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, color);

    EndMode3D();
}
