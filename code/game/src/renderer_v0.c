static Camera2D render_camera;
static float zoom_overlay_tran = 0.0f;

#define CAM_OVERLAY_ZOOM_LEVEL 0.80f
#define ALPHA(x) ColorAlpha(x, data->tran_time)

float zpl_lerp(float,float,float);
float zpl_to_degrees(float);

void DEBUG_draw_ground(uint64_t key, entity_view * data) {
    (void)key;
    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            int32_t size = view->chunk_size * WORLD_BLOCK_SIZE;
            int16_t offset = 0;
            
            float x = data->x * size + offset;
            float y = data->y * size + offset;
            
            RenderTexture2D tex = GetChunkTexture(key);
            DrawTextureRec(tex.texture, (Rectangle){0, 0, tex.texture.width, -tex.texture.height}, (Vector2){x, y}, ColorAlpha(WHITE, data->tran_time));
            
            if (zoom_overlay_tran > 0.02f) {
                DrawRectangleEco(x, y, size-offset, size-offset, ColorAlpha(ColorFromHSV(data->color, 0.13f, 0.89f), data->tran_time*zoom_overlay_tran*0.75f));
                
                DrawTextEco(TextFormat("%d %d", (int)data->x, (int)data->y), (int16_t)x+15, (int16_t)y+15, 200 , ColorAlpha(BLACK, data->tran_time*zoom_overlay_tran), 0.0); 
                
            }
        }break;
        
        default:break;
    }
}

void DEBUG_draw_entities(uint64_t key, entity_view * data) {
    uint16_t size = 16;
    uint16_t font_size = (uint16_t)lerp(4.0f, 32.0f, 0.5f/(float)render_camera.zoom);
    float font_spacing = 1.1f;
    float title_bg_offset = 4;
    float fixed_title_offset = 8;
    
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
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-fixed_title_offset, title_w*health+title_bg_offset, font_size*0.2f, ColorAlpha(RED, data->tran_time));
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing); 
            DrawCircleEco(x, y, size, ColorAlpha(YELLOW, data->tran_time));
        }break;
        case EKIND_MACRO_BOT: {
            float x = data->x;
            float y = data->y;
            const char *title = TextFormat("Bot %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(GRAY, data->tran_time));
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(BLACK, data->tran_time), font_spacing); 
            DrawCircleEco(x, y, size, ColorAlpha(PURPLE, data->tran_time));
        }break;
        case EKIND_ITEM: {
            float x = data->x;
            float y = data->y;
            DrawTexturePro(GetSpriteTexture2D(assets_find(data->asset)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, ALPHA(WHITE));
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
        default:break;
    }
}

void renderer_draw_v0(void) {
    render_camera.zoom = zpl_lerp(render_camera.zoom, target_zoom, 0.18);
    camera_update();
    
    camera game_camera = camera_get();
    render_camera.target = (Vector2){game_camera.x, game_camera.y};
    zoom_overlay_tran = zpl_lerp(zoom_overlay_tran, (target_zoom <= CAM_OVERLAY_ZOOM_LEVEL) ? 1.0f : 0.0f, GetFrameTime()*2.0f);
    
    
    ClearBackground(GetColor(0x222034));
    BeginMode2D(render_camera);
    game_world_view_active_entity_map(DEBUG_draw_ground);
    game_world_view_active_entity_map(DEBUG_draw_entities_low);
    game_world_view_active_entity_map(DEBUG_draw_entities);
    EndMode2D();
}

void renderer_init_v0(void) {
    render_camera.target = (Vector2){0.0f,0.0f};
    render_camera.offset = (Vector2){screenWidth >> 1, screenHeight >> 1};
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
