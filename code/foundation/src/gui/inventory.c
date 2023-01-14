#include "models/crafting.h"

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
    uint16_t craft_item;
} inv_keystate;

static inv_keystate player_inv = {0};
static inv_keystate storage_inv = {0};

bool inv_is_open = false;
bool inv_is_inside = false;
bool inv_is_storage_action = false;
bool inv_swap_storage = false;

// TODO(zaklaus): 
// TODO(zaklaus): MOVE TO COMMON UI MODULE
// TODO(zaklaus): 

typedef struct {
    float x, y;
} inv_draw_result;

static inline
int UIMeasureText(const char *text, int fontSize) {
    Vector2 vec = { 0.0f, 0.0f };
    
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        int defaultFontSize = 10;   // Default Font chars height in pixel
        int new_spacing = fontSize/defaultFontSize;
        
        vec = MeasureTextEx(GetFontDefault(), text, (float)fontSize, (float)new_spacing);
    }
    
    return (int)vec.x;
}

static inline
void UIDrawText(const char *text, float posX, float posY, int fontSize, Color color) {
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        Vector2 position = { (float)posX , (float)posY  };
        
        int defaultFontSize = 22;   // Default Font chars height in pixel
        int new_spacing = fontSize/defaultFontSize;
        
        DrawTextEx(GetFontDefault(), text, position, (float)fontSize , (float)new_spacing , color);
    }
}


static inline inv_draw_result
DrawColoredText(float xpos, float ypos, char const *text, Color color) {
    ZPL_ASSERT(text);
    UIDrawText(text, xpos, ypos, 22, color);
    
    char const *p = text;
    uint8_t newlines = 1;
    
    do {
        if (*p == '\n')
            ++newlines;
    } while (*p++ != 0);
    
    return (inv_draw_result){.x = xpos + UIMeasureText(text, 22), .y = ypos + 22*newlines};
}


static inline
inv_draw_result inventory_draw_crafting_btn(float xpos, float ypos, const char *name, uint16_t id, Color color) {
    float name_width=0.0f;
    char const *text = TextFormat("> %s", name);
    name_width = (float)UIMeasureText(text, 22);
    
    Color new_color = color;
    if (is_btn_pressed(xpos, ypos, name_width, 22, &new_color)) {
        inv_is_inside = true;
        player_inv.craft_item = id;
    }
    
    Color _c_compare_lol = BLACK;
    if (!zpl_memcompare(&color, &_c_compare_lol, sizeof(Color))) {
        new_color = BLACK;
    }
    
    inv_draw_result res = DrawColoredText(xpos, ypos, text, new_color);
    ypos = res.y;
    return res;
}

static inline
bool inventory_draw_crafting_list(float xpos, float ypos) {
    // NOTE(zaklaus): collect the list of supported recipes
    // TODO(zaklaus): too lazy, draw all recipes everywhere for now
    
    float start_xpos = xpos;
    float start_ypos = ypos;
    for (uint16_t i = 0; i < craft_get_num_recipes(); ++i) {
        asset_id id = craft_get_recipe_asset(i);
        inventory_draw_crafting_btn(start_xpos+1, ypos+1, asset_names[id], id, BLACK);
        inv_draw_result entry = inventory_draw_crafting_btn(start_xpos, ypos, asset_names[id], id, RAYWHITE);
        ypos = entry.y;
        xpos = zpl_max(xpos, entry.x);
    }
    
    return check_mouse_area(start_xpos, start_ypos, xpos-start_xpos, ypos-start_ypos) != DAREA_OUTSIDE;
}

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
    
    bool inside_craft = !is_player && inventory_draw_crafting_list(screenWidth/2.0f - 684, screenHeight/2.0f - 128);
    
    inv->is_inside = check_mouse_area(sx, sy, (float)grid_size, (float)grid_size) != DAREA_OUTSIDE;
    inv_is_inside |= inv->is_inside || inside_craft;
    
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
                Texture2D tex = GetSpriteTexture2D(assets_find(item->kind));
                float aspect = tex.width/(float)tex.height;
                float size = WORLD_BLOCK_SIZE * aspect;
                float ofs_x = (WORLD_BLOCK_SIZE-size)/2.0f;
                DrawTexturePro(tex, ASSET_SRC_RECT_TEX(tex.width, tex.height), ASSET_DST_RECT_TEX(x+ofs_x, y, size, WORLD_BLOCK_SIZE), (Vector2){0.5f,0.5f}, 0.0f, WHITE);
            }
            
            if (item->quantity > 1) {
                DrawTextEco(zpl_bprintf("%d", item->quantity), x+5, y+5, 16, RAYWHITE, 0.0f);
            }
            
            if (item->quantity > 0 && item->durability < 1.0f) {
                DrawRectangleEco(x, y+56, 64, 8, BLACK);
                DrawRectangleEco(x, y+56, 64*item->durability, 8, BlendColor(RED, GREEN, item->durability));
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
        Texture2D tex = GetSpriteTexture2D(assets_find(inv->held_item.kind));
        float aspect = tex.width/(float)tex.height;
        float size = WORLD_BLOCK_SIZE * aspect;
        float ofs_x = (WORLD_BLOCK_SIZE-size)/2.0f;
        DrawTexturePro(tex, ASSET_SRC_RECT_TEX(tex.width, tex.height), ASSET_DST_RECT_TEX(mpos.x+ofs_x, mpos.y, size, WORLD_BLOCK_SIZE), (Vector2){0.5f,0.5f}, 0.0f, ColorAlpha(WHITE, 0.8f));
        DrawTextEco(zpl_bprintf("%d", inv->held_item.quantity), mpos.x, mpos.y, 16, RAYWHITE, 0.0f);
        
        if (!inv->is_inside && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && !inv2->is_inside) {
            inv->drop_item = true;
            inv->item_is_held = false;
            inv_is_storage_action = inv == &storage_inv;
        }
    }
}

void inventory_reset_states(inv_keystate *ik) {
    ik->drop_item = false;
    ik->swap = false;
    ik->craft_item = 0;
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
    
    if (input_is_pressed(IN_TOGGLE_INV)) {
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
