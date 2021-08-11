
float zpl_lerp(float,float,float);
float zpl_to_degrees(float);

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

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

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
#if 0
            const char *title = TextFormat("Thing %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing); 
#endif
            DrawCircleEco(x, y, size, ColorAlpha(BLUE, data->tran_time));
        }break;
        case EKIND_PLAYER: {
            float x = data->x;
            float y = data->y;
            float health = (data->hp / data->max_hp);
#if 1
            const char *title = TextFormat("Player %d", key);
            int title_w = MeasureTextEco(title, font_size, font_spacing);
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-font_size-fixed_title_offset, title_w+title_bg_offset, font_size, ColorAlpha(BLACK, data->tran_time));
            DrawRectangleEco(x-title_w/2-title_bg_offset/2, y-size-fixed_title_offset, title_w*health+title_bg_offset, font_size*0.2f, ColorAlpha(RED, data->tran_time));
            DrawTextEco(title, x-title_w/2, y-size-font_size-fixed_title_offset, font_size, ColorAlpha(RAYWHITE, data->tran_time), font_spacing); 
#endif
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

void renderer_draw(void) {
    BeginDrawing();
    profile (PROF_RENDER) {
        ClearBackground(GetColor(0x222034));
        BeginMode2D(render_camera);
        game_world_view_active_entity_map(DEBUG_draw_ground);
        game_world_view_active_entity_map(DEBUG_draw_entities_low);
        game_world_view_active_entity_map(DEBUG_draw_entities);
        EndMode2D();        
        display_conn_status();
    }
    debug_draw();
    EndDrawing();
}