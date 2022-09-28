typedef struct {
    uint8_t selected_item;
    bool drop_item;
    
    bool item_is_held;
    uint8_t held_item_idx;
    Item held_item;
    
    bool is_inside;
    bool storage_action;
    bool swap;
    uint8_t swap_from;
    uint8_t swap_to;
} inv_keystate;

static inv_keystate player_inv = {0};
static inv_keystate storage_inv = {0};

bool inv_is_open = false;
bool inv_is_inside = false;
bool inv_is_storage_action = false;
bool inv_swap_storage = false;

void inventory_draw_panel(entity_view *e, bool is_player, float sx, float sy){
    if (!e->has_items && is_player)
        return;
    if (!e->has_storage_items && !is_player)
        return;
    
    float x = sx;
    float y = sy;
    
    const int32_t grid_size = (is_player) ? (64*3) : (64*4);
    const int32_t inv_size = (is_player) ? ITEMS_INVENTORY_SIZE : ITEMS_CONTAINER_SIZE;
    const int32_t inv_cols = (is_player) ? 3 : 4;
    
    inv_keystate *inv = (!is_player) ? &storage_inv : &player_inv;
    inv_keystate *inv2 = (is_player) ? &storage_inv : &player_inv;
    
    inv->is_inside = check_mouse_area(sx, sy, (float)grid_size, (float)grid_size) != DAREA_OUTSIDE;
    inv_is_inside |= inv->is_inside;
    
    for (int32_t i = 0; i < inv_size; i += 1) {
        {
            debug_area_status area = check_mouse_area(x, y, 64, 64);
            Color color = RAYWHITE;
            Item *item = (is_player) ? &e->items[i] : &e->storage_items[i];
            
            if (area == DAREA_HOVER) {
                color = YELLOW;
            } else if (area == DAREA_PRESS && inv2->item_is_held){
                color = VIOLET;
                inv_swap_storage = true;
                inv_is_storage_action = true;
                inv->item_is_held = false;
                inv2->item_is_held = false;
                inv->selected_item = i;
                inv->swap = true;
                inv->swap_from = inv2->held_item_idx;
                inv->swap_to = i;
            } else if (area == DAREA_PRESS && !inv->item_is_held && !inv2->item_is_held) {
                color = VIOLET;
                inv_is_storage_action = true;
                inv->selected_item = i;
            } else if (area == DAREA_PRESS && inv->item_is_held) {
                color = VIOLET;
                inv_is_storage_action = true;
                inv->selected_item = i;
                inv->item_is_held = false;
                inv->swap = true;
                inv->swap_from = inv->held_item_idx;
                inv->swap_to = i;
            } else if (area == DAREA_HELD && item->quantity > 0 && !inv->item_is_held && !inv2->item_is_held) {
                inv_is_storage_action = true;
                inv->selected_item = i;
                inv->item_is_held = true;
                inv->held_item = *item;
                inv->held_item_idx = i;
            } else if (i == inv->selected_item) {
                color = RED;
            }
            
            DrawRectangleLinesEco(x, y, 64, 64, color);
            
            if (item->quantity > 0) {
                DrawTexturePro(GetSpriteTexture2D(assets_find(item->kind)), ASSET_SRC_RECT(), ASSET_DST_RECT(x,y), (Vector2){0.5f,0.5f}, 0.0f, WHITE);
                DrawTextEco(zpl_bprintf("%d", item->quantity), x+5, y+5, 16, RAYWHITE, 0.0f); 
            }
        }
        x += 64;
        
        if ((i+1) % inv_cols == 0) {
            x = sx;
            y += 64;
        }
    }
    
    // NOTE(zaklaus): switch it off if is_player
    if (is_player)
        inv_is_storage_action = false;
}

void inventory_render_held_item(bool is_player){
    inv_keystate *inv = (!is_player) ? &storage_inv : &player_inv;
    inv_keystate *inv2 = (is_player) ? &storage_inv : &player_inv;
    
    if (inv->item_is_held) {
        Vector2 mpos = GetMousePosition();
        mpos.x -= 32;
        mpos.y -= 32;
        DrawTexturePro(GetSpriteTexture2D(assets_find(inv->held_item.kind)), ASSET_SRC_RECT(), ASSET_DST_RECT(mpos.x, mpos.y), (Vector2){0.5f,0.5f}, 0.0f, ColorAlpha(WHITE, 0.8f));
        DrawTextEco(zpl_bprintf("%d", inv->held_item.quantity), mpos.x, mpos.y, 16, RAYWHITE, 0.0f); 
        
        if (!inv->is_inside && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && !inv2->is_inside) {
            inv->drop_item = true;
            inv->item_is_held = false;
            inv_is_storage_action = true;
        }
    }
}

void inventory_reset_states(inv_keystate *ik) {
    ik->drop_item = false;
    ik->swap = false;
}

void inventory_draw() {
    inv_is_storage_action = false;
    inv_is_inside = false;
    inv_swap_storage = false;
    inventory_reset_states(&player_inv);
    inventory_reset_states(&storage_inv);
    
    camera cam = camera_get();
    entity_view *e = game_world_view_active_get_entity(cam.ent_id);
    if (!e || !e->has_items) return;
    
    if (IsKeyPressed(KEY_TAB)) {
        inv_is_open = !inv_is_open;
    }
    
    if (!inv_is_open || build_is_in_draw_mode) {
        return;
    }
    
    inventory_draw_panel(e, true, screenWidth/2.0f + 128, screenHeight/2.0f - 96);
    inventory_draw_panel(e, false, screenWidth/2.0f - 384, screenHeight/2.0f - 128);
    
    inventory_render_held_item(true);
    inventory_render_held_item(false);
}
