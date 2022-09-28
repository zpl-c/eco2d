static Camera3D render_camera;
static float cam_zoom = 1.5f;

float zpl_lerp(float,float,float);
float zpl_to_degrees(float);

void DEBUG_draw_ground(uint64_t key, entity_view * data) {
    switch (data->kind) {
        case EKIND_CHUNK: {
            world_view *view = game_world_view_get_active();
            float size = (float)(view->chunk_size * WORLD_BLOCK_SIZE);
            float half_size = size / 2.0f;
            float half_block_size = (float)(WORLD_BLOCK_SIZE >> 1);
            int16_t offset = 0;
            
            float x = data->x * size + offset;
            float y = data->y * size + offset;
            
            RenderTexture2D tex = GetChunkTexture(key);
            DrawCubeTexture(tex.texture, (Vector3){x+half_size, 0.0f, y+half_size}, size, 0.01f, size, WHITE);
            
            for (size_t ty = 0; ty < view->chunk_size; ty++) {
                for (size_t tx = 0; tx < view->chunk_size; tx++) {
                    block_id blk_id = data->outer_blocks[(ty*view->chunk_size)+tx];
                    if (blk_id != 0) {
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
        case EKIND_VEHICLE: {
            float x = data->x;
            float y = data->y;
            float const w = 280;
            float const h = 150;
            EcoDrawCube((Vector3){x,15.0f,y}, w/2.0f, 10.f, h/2.0f, -zpl_to_degrees(data->heading), ColorAlpha(RED, data->tran_time));
        }break;
        case EKIND_CHUNK:break;
        
        default:{
            float x = data->x;
            float y = data->y;
            EcoDrawCube((Vector3){x, 15.0f, y}, 16, 16, 16, 0.0f, ColorAlpha(PINK, data->tran_time));
        }break;
    }
}

void renderer_draw(void) {
    cam_zoom = zpl_min(zpl_lerp(cam_zoom, target_zoom, GetFrameTime()*2.18f), 9.98f);
    camera_update();
    
    camera game_camera = camera_get();
#if 1
    render_camera.position = (Vector3){(float)game_camera.x, 260.0f*(10.0f-cam_zoom), (float)game_camera.y+50.0f*(10.0f-cam_zoom/2.0f)};
    render_camera.target = (Vector3){(float)game_camera.x, 0.0f, (float)game_camera.y};
#else
    UpdateCamera(&render_camera);
#endif
    
    ClearBackground(GetColor(0x222034));
    BeginMode3D(render_camera);
    game_world_view_active_entity_map(DEBUG_draw_ground);
    game_world_view_active_entity_map(DEBUG_draw_entities);
    EndMode3D();
}

void renderer_init(void) {
    render_camera.up = (Vector3){0.0f,0.0f,-1.0f};
    render_camera.fovy = 45.f;
    render_camera.projection = CAMERA_PERSPECTIVE;
#if 0
    SetCameraMode(render_camera, CAMERA_ORBITAL);
    render_camera.position = (Vector3){10,10,10};
    render_camera.target = (Vector3){0};
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


void renderer_debug_draw(void) {
    
}

float renderer_zoom_get(void) {
    return cam_zoom;
}

void renderer_draw_single(float x, float y, asset_id id, Color color) {
    (void)color;
    BeginMode3D(render_camera);
    EcoDrawCube((Vector3){x, 15.0f, y}, 16, 16, 16, 0.0f, PINK);
    EndMode3D();
}
