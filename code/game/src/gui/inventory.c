static uint8_t inv_selected_item = 0;
static bool inv_drop_item = false;
bool inv_is_open = false;

static bool inv_item_is_held = false;
static uint8_t inv_held_item_idx = 0;
static ItemDrop inv_held_item = {0};

static bool inv_swap = false;
static uint8_t inv_swap_from = 0;
static uint8_t inv_swap_to = 0;
bool inv_is_inside = false;

void inventory_draw() {
    inv_drop_item = false;
    inv_swap = false;
    
    camera cam = camera_get();
    entity_view *e = game_world_view_active_get_entity(cam.ent_id);
    if (!e || !e->has_items) return;
    
    if (IsKeyPressed(KEY_TAB)) {
        inv_is_open = !inv_is_open;
    }
    
    if (!inv_is_open || build_is_in_draw_mode) {
        return;
    }
    
    float sx = screenWidth/2.0f + 128;
    float sy = screenHeight/2.0f - 96;
    
    float x = sx;
    float y = sy;
    
    inv_is_inside = check_mouse_area(sx, sy, 64*3, 64*3) != DAREA_OUTSIDE;
    
    for (int32_t i = 0; i < ITEMS_INVENTORY_SIZE; i += 1) {
        {
            debug_area_status area = check_mouse_area(x, y, 64, 64);
            Color color = RAYWHITE;
            ItemDrop *item = &e->items[i];
            
            if (area == DAREA_HOVER) {
                color = YELLOW;
            } else if (area == DAREA_PRESS && !inv_item_is_held) {
                color = VIOLET;
                inv_selected_item = i;
            } else if (area == DAREA_PRESS && inv_item_is_held) {
                color = VIOLET;
                inv_selected_item = i;
                inv_item_is_held = false;
                inv_swap = true;
                inv_swap_from = inv_held_item_idx;
                inv_swap_to = i;
            } else if (area == DAREA_HELD && item->quantity > 0 && !inv_item_is_held) {
                inv_selected_item = i;
                inv_held_item = *item;
                inv_item_is_held = true;
                inv_held_item_idx = i;
            } else if (i == inv_selected_item) {
                color = RED;
            }
            
            DrawRectangleLinesEco(x, y, 64, 64, color);
            
            if (item->quantity > 0) {
                DrawTexturePro(GetSpriteTexture2D(assets_find(item->kind)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, WHITE);
                DrawTextEco(zpl_bprintf("%d", item->quantity), x+5, y+5, 16, RAYWHITE, 0.0f); 
            }
        }
        x += 64;
        
        if ((i+1) % 3 == 0) {
            x = sx;
            y += 64;
        }
    }
    
    if (inv_item_is_held) {
        Vector2 mpos = GetMousePosition();
        mpos.x -= 32;
        mpos.y -= 32;
        DrawTexturePro(GetSpriteTexture2D(assets_find(inv_held_item.kind)), ASSET_SRC_RECT(), ASSET_DST_RECT(mpos.x, mpos.y), (Vector2){0.5f,0.5f}, 0.0f, ColorAlpha(WHITE, 0.8f));
        DrawTextEco(zpl_bprintf("%d", inv_held_item.quantity), mpos.x, mpos.y, 16, RAYWHITE, 0.0f); 
        
        debug_area_status area = check_mouse_area(sx, sy, 64*3, 64*3);
        if (area == DAREA_OUTSIDE && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            inv_drop_item = true;
            inv_item_is_held = false;
        }
    }
}
