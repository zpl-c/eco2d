static Camera3D render_camera;
static float zoom_overlay_tran = 0.0f;
static float cam_zoom = 1.5f;

#define CAM_OVERLAY_ZOOM_LEVEL 0.80f


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
            int32_t half_size = size >> 1;
            int16_t offset = 0;
            
            float x = data->x * size + offset;
            float y = data->y * size + offset;
            
            RenderTexture2D tex = GetChunkTexture(key);
            DrawCubeTexture(tex.texture, (Vector3){x+half_size, 0.0f, y+half_size}, size, 1.0f, size, WHITE);
        }break;
        
        default:break;
    }
}

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

void DEBUG_draw_entities(uint64_t key, entity_view * data) {
    (void)key;
    uint16_t size = 16;
    uint16_t ground_offset = 30;
    
    switch (data->kind) {
        
        case EKIND_DEMO_NPC: {
            float x = data->x;
            float y = data->y;
            DrawSphere((Vector3){x,ground_offset,y}, size, ColorAlpha(BLUE, data->tran_time));
        }break;
        
        case EKIND_PLAYER: {
            float x = data->x;
            float y = data->y;
            DrawSphere((Vector3){x,ground_offset,y}, size, ColorAlpha(YELLOW, data->tran_time));
        }break;
        
        case EKIND_MACRO_BOT: {
            float x = data->x;
            float y = data->y;
            DrawSphere((Vector3){x,ground_offset,y}, size, ColorAlpha(PURPLE, data->tran_time));
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
            float const w = 280;
            float const h = 150;
            EcoDrawCube((Vector3){x,15.0f,y}, w/2.0f, 10.f, h/2.0f, -zpl_to_degrees(data->heading), ColorAlpha(RED, data->tran_time));
        }break;
        default:break;
    }
}


void renderer_draw(void) {
    cam_zoom = zpl_min(zpl_lerp(cam_zoom, target_zoom, 0.18), 9.98f);
    camera_update();
    
    camera game_camera = camera_get();
#if 1
    render_camera.position = (Vector3){game_camera.x, 260.0f*(10.0f-cam_zoom), game_camera.y+50.0f*(10.0f-cam_zoom/2.0f)};
    render_camera.target = (Vector3){game_camera.x, 0.0f, game_camera.y};
#else
    UpdateCamera(&render_camera);
#endif
    
    ClearBackground(GetColor(0x222034));
    BeginMode3D(render_camera);
    game_world_view_active_entity_map(DEBUG_draw_ground);
    game_world_view_active_entity_map(DEBUG_draw_entities_low);
    game_world_view_active_entity_map(DEBUG_draw_entities);
    EndMode3D();
}

void renderer_init(void) {
    render_camera.up = (Vector3){0.0f,0.0f,-1.0f};
    render_camera.fovy = 45.f;
    render_camera.projection = CAMERA_PERSPECTIVE;
#if 1
    SetCameraMode(render_camera, CAMERA_MODE_STATIONARY);
#else
    render_camera.position = (Vector3){10,10,10};
    render_camera.target = (Vector3){0};
    SetCameraMode(render_camera, CAMERA_ORBITAL);
#endif
    
    // NOTE(zaklaus): Paint the screen before we load the game
    // TODO(zaklaus): Render a cool loading screen background maybe? :wink: :wink:
    
    BeginDrawing();
    ClearBackground(GetColor(0x222034));
    
    char const *loading_text = "zpl.eco2d (in 3d) is loading...";
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
