static inline
void int_to_hex_color(uint32_t color, char *text);
static inline
int GuiDropdownBoxEco(Rectangle bounds, char const *text, char const *caption, int *active, bool editMode);
static inline
bool GuiValueBoxEco(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode);

static inline
bool IsCtrlAcceleratorPressed(char key);

void texed_draw_topbar(zpl_aabb2 r) {
    zpl_aabb2 zoom_ctrl_r = zpl_aabb2_cut_left(&r, 150.0f);
    
    zoom = GuiSlider(aabb2_ray(zoom_ctrl_r), "zoom: ", zpl_bprintf("%.02f x", zoom), zoom, 1.0f, 16.0f);
    if (zoom != old_zoom) {
        ctx.is_saved = false;
        old_zoom = zoom;
    }
    
    zpl_aabb2_cut_left(&r, 100.0f);
    
    zpl_aabb2 render_tiles_ctrl_r = zpl_aabb2_cut_left(&r, 150.0f);
    
    render_tiles = (int)GuiSlider(aabb2_ray(render_tiles_ctrl_r), "tiles: ", zpl_bprintf("%d", render_tiles+1), render_tiles, 0.0f, 50.0f);
    
    zpl_aabb2_cut_left(&r, 100.0f);
    
    zpl_aabb2 new_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    static bool new_pending = false;
    
    if (GuiButton(aabb2_ray(new_prj_r), "NEW") || IsCtrlAcceleratorPressed('n')) {
        if (ctx.is_saved) {
            texed_clear();
            texed_new(TD_DEFAULT_IMG_WIDTH, TD_DEFAULT_IMG_HEIGHT);
        } else {
            new_pending = true;
            texed_msgbox_init("Discard unsaved work?", "You have an unsaved work! Do you want to proceed?", "OK;Cancel");
        }
    }
    
    if (new_pending && ctx.msgbox.result != -1) {
        new_pending = false;
        if (ctx.msgbox.result == 1) {
            texed_clear();
            texed_new(TD_DEFAULT_IMG_WIDTH, TD_DEFAULT_IMG_HEIGHT);
        }
        ctx.msgbox.result = -1; // NOTE(zaklaus): ensure we don't re-trigger this branch next frame
    }
    
    zpl_aabb2 load_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    static bool load_pending = false;
    
    if (GuiButton(aabb2_ray(load_prj_r), "LOAD") || IsCtrlAcceleratorPressed('o')) {
        load_pending = true;
        if (ctx.is_saved) {
            ctx.fileDialog.fileDialogActive = true;
        } else {
            texed_msgbox_init("Discard unsaved work?", "You have an unsaved work! Do you want to proceed?", "OK;Cancel");
        }
    }
    
    if (ctx.fileDialog.SelectFilePressed && load_pending) {
        ctx.fileDialog.SelectFilePressed = false;
        if (IsFileExtension(ctx.fileDialog.fileNameText, ".ecotex")) {
            strcpy(filename, GetFileNameWithoutExt(ctx.fileDialog.fileNameText));
            ctx.filepath = filename;
            load_pending = false;
            texed_load();
        } else {
            ctx.fileDialog.fileDialogActive = true;
        }
    }
    
    if (load_pending && ctx.msgbox.result != -1) {
        if (ctx.msgbox.result == 1) {
            ctx.fileDialog.fileDialogActive = true;
        }
        else load_pending = false;
        ctx.msgbox.result = -1; // NOTE(zaklaus): ensure we don't re-trigger this branch next frame
    }
    
    zpl_aabb2 save_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    static bool save_as_pending = false;
    
    if (GuiButton(aabb2_ray(save_prj_r), "SAVE") || IsCtrlAcceleratorPressed('s')) {
        if (ctx.filepath == NULL) {
            save_as_pending = true;
            ctx.fileDialog.fileDialogActive = true;
        } else {
            texed_save();
        }
    }
    
    zpl_aabb2 save_as_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    
    if (GuiButton(aabb2_ray(save_as_prj_r), "SAVE AS")) {
        save_as_pending = true;
        ctx.fileDialog.fileDialogActive = true;
    }
    
    if (ctx.fileDialog.SelectFilePressed && save_as_pending) {
        ctx.fileDialog.SelectFilePressed = false;
        if (TextLength(ctx.fileDialog.fileNameText)) {
            strcpy(filename, GetFileNameWithoutExt(ctx.fileDialog.fileNameText));
            ctx.filepath = filename;
            save_as_pending = false;
            texed_save();
        } else {
            ctx.fileDialog.fileDialogActive = true;
        }
    }
    
    zpl_aabb2 split_r = zpl_aabb2_cut_left(&r, 5.0f);
    split_r = zpl_aabb2_contract(&split_r, 2.0f);
    DrawAABB(split_r, BLACK);
    
    zpl_aabb2 exports_r = zpl_aabb2_cut_left(&r, 240.0f);
    GuiSetState(ctx.filepath ? GUI_STATE_NORMAL : GUI_STATE_DISABLED);
    zpl_aabb2 export_code_r = zpl_aabb2_cut_left(&exports_r, 120.0f);
    
    if (GuiButton(aabb2_ray(export_code_r), "BUILD TEXTURE")) {
        texed_export_cc(ctx.filepath);
    }
    
    zpl_aabb2 export_img_r = zpl_aabb2_cut_left(&exports_r, 120.0f);
    
    if (GuiButton(aabb2_ray(export_img_r), "EXPORT AS IMAGE")) {
        texed_export_png(ctx.filepath);
    }
    GuiSetState(GUI_STATE_NORMAL);
    
    
    zpl_aabb2 prj_name_r = zpl_aabb2_cut_right(&r, 200.0f);
    zpl_aabb2_cut_right(&prj_name_r, 15.0f);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    GuiDrawText(zpl_bprintf("Project: %s%s", ctx.filepath ? ctx.filepath : "(unnamed)", ctx.is_saved ? "" : "*"), GetTextBounds(LABEL, aabb2_ray(prj_name_r)), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(GREEN, guiAlpha));
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
}

void texed_draw_oplist_pane(zpl_aabb2 r) {
    // NOTE(zaklaus): add operator
    {
        zpl_aabb2 add_op_r = zpl_aabb2_cut_right(&r, 180.0f);
        DrawAABB(add_op_r, GetColor(0x122025));
        add_op_r = zpl_aabb2_contract(&add_op_r, 3.0f);
        Rectangle panel_rec = aabb2_ray(add_op_r);
        static Vector2 panel_scroll = {99, -20};
        float list_y = (DEF_OPS_LEN) * 22.5f;
        if (list_y >= (add_op_r.max.y-add_op_r.min.y)) add_op_r.min.x += 12.0f;
        else add_op_r.min.x += 2.0f;
        add_op_r.max.y = add_op_r.min.y + list_y;
        
        Rectangle view = GuiScrollPanel(panel_rec, aabb2_ray(add_op_r), &panel_scroll);
        
        BeginScissorMode(view.x, view.y, view.width, view.height);
        
        for (int i = 0; i < DEF_OPS_LEN; i += 1) {
            if (default_ops[i].is_locked) continue;
            
            zpl_aabb2 add_op_btn_r = zpl_aabb2_cut_top(&add_op_r, 22.5f);
            add_op_btn_r.min.y += panel_scroll.y;
            add_op_btn_r.max.y += panel_scroll.y;
            zpl_aabb2_cut_bottom(&add_op_btn_r, 2.5f);
            if (GuiButton(aabb2_ray(add_op_btn_r), default_ops[i].name)) {
                texed_add_op(default_ops[i].kind);
            }
        }
        
        EndScissorMode();
    }
    
    // NOTE(zaklaus): recalculate height based on ops count
    Rectangle panel_rec = aabb2_ray(r);
    static Vector2 panel_scroll = {99, -20};
    float list_y = zpl_array_count(ctx.ops)*25.0f;
    if (list_y >= (r.max.y-r.min.y)) r.min.x += 12.0f;
    else r.min.x += 2.0f;
    r.max.y = r.min.y + list_y;
    
    Rectangle view = GuiScrollPanel(panel_rec, aabb2_ray(r), &panel_scroll);
    
    BeginScissorMode(view.x, view.y, view.width, view.height);
    
    // NOTE(zaklaus): operator list
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        zpl_aabb2 op_item_r = zpl_aabb2_cut_top(&r, 25.0f);
        op_item_r.min.y += panel_scroll.y;
        op_item_r.max.y += panel_scroll.y;
        zpl_aabb2_cut_top(&op_item_r, 2.5f);
        zpl_aabb2_cut_bottom(&op_item_r, 2.5f);
        Rectangle list_item = aabb2_ray(op_item_r);
        Color bg_color = BLUE;
        
        if (ctx.selected_op == i) {
            bg_color = GREEN;
        }
        else if (ctx.ops[i].is_hidden) {
            bg_color = RED;
        }
        else if (ctx.ops[i].is_locked) {
            bg_color = SKYBLUE;
        }
        
        DrawRectangleRec(list_item, ColorAlpha(bg_color, 0.4f));
        
        zpl_aabb2 swap_r = zpl_aabb2_cut_left(&op_item_r, 50.0f);
        Rectangle list_text = aabb2_ray(op_item_r);
        
        zpl_aabb2_cut_right(&swap_r, 5.0f);
        zpl_aabb2 swap_top = zpl_aabb2_cut_left(&swap_r, aabb2_ray(swap_r).width/2.0f);
        zpl_aabb2 swap_bottom = swap_r;
        
        if (i == 0 || ctx.ops[i].is_locked || (i > 0 && ctx.ops[i-1].is_locked)) GuiSetState(GUI_STATE_DISABLED);
        if (GuiButton(aabb2_ray(swap_top), "#121#")) {
            texed_swp_op(i, i-1);
        }
        GuiSetState(GUI_STATE_NORMAL);
        
        if (ctx.ops[i].is_locked || (i+1 < zpl_array_count(ctx.ops) && ctx.ops[i+1].is_locked) || i+1 >= zpl_array_count(ctx.ops)) GuiSetState(GUI_STATE_DISABLED);
        if (GuiButton(aabb2_ray(swap_bottom), "#120#")) {
            texed_swp_op(i, i+1);
        }
        GuiSetState(GUI_STATE_NORMAL);
        
        zpl_aabb2 remove_r = zpl_aabb2_cut_right(&op_item_r, 20.0f);
        
        if (ctx.ops[i].is_locked) GuiSetState(GUI_STATE_DISABLED);
        if (GuiButton(aabb2_ray(remove_r), "#143#")) {
            texed_rem_op(i);
        }
        
        zpl_aabb2 hidden_r = zpl_aabb2_cut_right(&op_item_r, 20.0f);
        
        if (!default_ops[texed_find_op(ctx.ops[i].kind)].is_locked) GuiSetState(GUI_STATE_NORMAL);
        if (GuiButton(aabb2_ray(hidden_r), ctx.ops[i].is_hidden ? "#45#" : "#44#")) {
            ctx.ops[i].is_hidden = !ctx.ops[i].is_hidden;
            texed_repaint_preview();
        }
        GuiSetState(GUI_STATE_NORMAL);
        
        zpl_aabb2 lock_r = zpl_aabb2_cut_right(&op_item_r, 20.0f);
        
        if (default_ops[texed_find_op(ctx.ops[i].kind)].is_locked) GuiSetState(GUI_STATE_DISABLED);
        if (GuiButton(aabb2_ray(lock_r), ctx.ops[i].is_locked ? "#137#" : "#138#")) {
            ctx.ops[i].is_locked = !ctx.ops[i].is_locked;
            ctx.is_saved = false;
        }
        GuiSetState(GUI_STATE_NORMAL);
        
        if (ctx.selected_op == i) GuiSetState(GUI_STATE_DISABLED);
        zpl_aabb2 select_r = zpl_aabb2_cut_right(&op_item_r, 20.0f);
        
        if (GuiButton(aabb2_ray(select_r), "#141#")) {
            ctx.selected_op = i;
            ctx.is_saved = false;
        }
        GuiSetState(GUI_STATE_NORMAL); 
        
        GuiDrawText(zpl_bprintf("%s %s", ctx.ops[i].name, ctx.ops[i].is_locked ? "(locked)" : ""), GetTextBounds(LABEL, list_text), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(RAYWHITE, guiAlpha));
    }
    
    EndScissorMode();
}

void texed_draw_props_pane(zpl_aabb2 r) {
    if (zpl_array_count(ctx.ops) == 0) {
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
        GuiDrawText("No operation is selected!", GetTextBounds(LABEL, aabb2_ray(r)), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(RAYWHITE, guiAlpha));
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
        return;
    }
    
    td_op *op = &ctx.ops[ctx.selected_op];
    Rectangle dims = aabb2_ray(r);
    
    zpl_aabb2 column_1_r = zpl_aabb2_cut_left(&r, dims.width/2.0f);
    zpl_aabb2 column_2_r = r;
    float prop_height = 25.0f;
    int prop_column_treshold = (int)zpl_floor(dims.height / prop_height);
    
    for (int i = 0; i < op->num_params; i += 1) {
        td_param *p = &op->params[i];
        zpl_aabb2 *c = (i >= prop_column_treshold) ? &column_2_r : &column_1_r;
        zpl_aabb2 item = zpl_aabb2_cut_top(c, prop_height);
        zpl_aabb2_cut_bottom(&item, 5.0f);
        zpl_aabb2 label_r = zpl_aabb2_cut_left(&item, dims.width/6.0f);
        zpl_aabb2 tbox_r = item;
        
        GuiDrawText(zpl_bprintf("%s: ", p->name ? p->name : "prop"), GetTextBounds(LABEL, aabb2_ray(label_r)), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(RAYWHITE, guiAlpha));
        
        static bool is_color_editing = false;
        if (is_color_editing) GuiLock();
        
        switch (p->kind) {
            case TPARAM_COLOR: {
                if (is_color_editing) GuiUnlock();
                if (GuiTextBoxEx(aabb2_ray(tbox_r), p->str, 1000, p->edit_mode)) {
                    p->edit_mode = true;
                    is_color_editing = true;
                }
                
                if (p->edit_mode) {
                    zpl_aabb2 extra_r = zpl_aabb2_cut_top(c, prop_height*4.0f + 50.0f);
                    zpl_aabb2_cut_bottom(&extra_r, 50.0f);
                    zpl_aabb2_cut_left(&extra_r, dims.width/6.0f);
                    DrawRectangleRec(aabb2_ray(extra_r), GRAY);
                    
                    zpl_aabb2 ok_r = zpl_aabb2_cut_left(&extra_r, 50.0f);
                    p->color = GuiColorPicker(aabb2_ray(extra_r), p->color);
                    
                    if (GuiButton(aabb2_ray(ok_r), "OK")) {
                        GuiUnlock();
                        p->edit_mode = false;
                        is_color_editing = false;
                        int_to_hex_color(ColorToInt(p->color), p->str);
                        texed_repaint_preview();
                    }
                }
                if (is_color_editing) GuiLock();
            }break;
            case TPARAM_SLIDER: {
                p->flt = GuiSlider(aabb2_ray(tbox_r), NULL, zpl_bprintf("%.02f", p->flt), p->flt, 0.0f, 1.0f);
                if (p->old_flt != p->flt) {
                    sprintf(p->str, "%f", p->flt);
                    p->old_flt = p->flt;
                    texed_repaint_preview();
                }
            }break;
            case TPARAM_INT:
            case TPARAM_COORD: {
                if (GuiValueBoxEco(aabb2_ray(tbox_r), NULL, &p->i32, INT32_MIN, INT32_MAX, p->edit_mode)) {
                    p->edit_mode = !p->edit_mode;
                    
                    if (!p->edit_mode) {
                        sprintf(p->str, "%d", p->i32);
                        texed_repaint_preview();
                    } else if (IsKeyDown(KEY_LEFT_SHIFT)) {
                        p->i32 = 0;
                    }
                };
            }break;
            default: {
                if (GuiTextBoxEx(aabb2_ray(tbox_r), p->str, 1000, p->edit_mode)) {
                    p->edit_mode = !p->edit_mode;
                    
                    if (!p->edit_mode)
                        texed_repaint_preview();
                }
            }break;
        };
        
        if (is_color_editing) GuiUnlock();
    }
}

zpl_global const char zpl__num_to_char_table[] = "0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"@$";


static inline
void int_to_hex_color(uint32_t value, char *string) {
    char *buf = string;
    
    if (value) {
        while (value > 0) {
            *buf++ = zpl__num_to_char_table[value % 16];
            value /= 16;
        }
    } else {
        *buf++ = '0';
    }
    *buf = '\0';
    
    zpl_strrev(string);
}


// Dropdown Box control
// NOTE: Returns mouse click
static inline
int GuiDropdownBoxEco(Rectangle bounds, char const *text, char const *caption, int *active, bool editMode)
{
    GuiControlState state = guiState;
    int itemSelected = *active;
    int itemFocused = -1;
    
    // Get substrings items from text (items pointers, lengths and count)
    int itemsCount = 0;
    const char **items = GuiTextSplit(text, &itemsCount, NULL);
    
    Rectangle boundsOpen = bounds;
    boundsOpen.height = (itemsCount + 1)*(bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));
    
    Rectangle itemBounds = bounds;
    
    bool pressed = false;       // Check mouse button pressed
    
    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked && (itemsCount > 1))
    {
        Vector2 mousePoint = GetMousePosition();
        
        if (editMode)
        {
            state = GUI_STATE_PRESSED;
            
            // Check if already selected item has been pressed again
            if (CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
            
            // Check focused and selected item
            for (int i = 0; i < itemsCount; i++)
            {
                // Update item rectangle y position for next item
                itemBounds.y += (bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));
                
                if (CheckCollisionPointRec(mousePoint, itemBounds))
                {
                    itemFocused = i;
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        itemSelected = i;
                        pressed = true;     // Item selected, change to editMode = false
                    }
                    break;
                }
            }
            
            itemBounds = bounds;
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    pressed = true;
                    state = GUI_STATE_PRESSED;
                }
                else state = GUI_STATE_FOCUSED;
            }
        }
    }
    //--------------------------------------------------------------------
    
    // Draw control
    //--------------------------------------------------------------------
    if (editMode) GuiPanel(boundsOpen);
    
    GuiDrawRectangle(bounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER + state*3)), guiAlpha), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE + state*3)), guiAlpha));
    
    GuiDrawText(caption, GetTextBounds(DEFAULT, bounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + state*3)), guiAlpha));
    
    if (editMode)
    {
        // Draw visible items
        for (int i = 0; i < itemsCount; i++)
        {
            // Update item rectangle y position for next item
            itemBounds.y += (bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));
            
            if (i == itemSelected)
            {
                GuiDrawRectangle(itemBounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER_COLOR_PRESSED)), guiAlpha), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE_COLOR_PRESSED)), guiAlpha));
                GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_PRESSED)), guiAlpha));
            }
            else if (i == itemFocused)
            {
                GuiDrawRectangle(itemBounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER_COLOR_FOCUSED)), guiAlpha), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE_COLOR_FOCUSED)), guiAlpha));
                GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_FOCUSED)), guiAlpha));
            }
            else GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL)), guiAlpha));
        }
    }
    
    // TODO: Avoid this function, use icon instead or 'v'
    DrawTriangle(RAYGUI_CLITERAL(Vector2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING), bounds.y + bounds.height/2 - 2 },
                 RAYGUI_CLITERAL(Vector2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING) + 5, bounds.y + bounds.height/2 - 2 + 5 },
                 RAYGUI_CLITERAL(Vector2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING) + 10, bounds.y + bounds.height/2 - 2 },
                 Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + (state*3))), guiAlpha));
    
    //GuiDrawText("v", RAYGUI_CLITERAL(Rectangle){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING), bounds.y + bounds.height/2 - 2, 10, 10 },
    //            GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------
    
    Vector2 mousePoint = GetMousePosition();
    
    // Check if mouse has been pressed or released outside limits
    if (!CheckCollisionPointRec(mousePoint, boundsOpen))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            return 2;
        }
    }
    
    *active = itemSelected;
    return pressed;
}

#define TD_UI_MSGBOX_WIDTH 320
#define TD_UI_MSGBOX_HEIGHT 200

void texed_draw_msgbox(zpl_aabb2 r) {
    if (!ctx.msgbox.visible) return;
    DrawRectangle(r.min.x, r.min.y, r.max.x, r.max.y, Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 0.85f));
    
    Rectangle rec = {
        r.max.x/2.0f - TD_UI_MSGBOX_WIDTH/2.0f,
        r.max.y/2.0f - TD_UI_MSGBOX_HEIGHT/2.0f,
        TD_UI_MSGBOX_WIDTH,
        TD_UI_MSGBOX_HEIGHT,
    };
    
    ctx.msgbox.result = GuiMessageBox(rec, ctx.msgbox.title, ctx.msgbox.message, ctx.msgbox.buttons);
    if (ctx.msgbox.result != -1) {
        ctx.msgbox.visible = false;
    }
}

static inline
bool IsCtrlAcceleratorPressed(char key) {
    return (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && (char)GetKeyPressed() == key;
}

static inline
bool GuiValueBoxEco(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode) {
#if !defined(VALUEBOX_MAX_CHARS)
#define VALUEBOX_MAX_CHARS  32
#endif
    
    static int framesCounter = 0;           // Required for blinking cursor
    
    GuiControlState state = guiState;
    bool pressed = false;
    
    char textValue[VALUEBOX_MAX_CHARS + 1] = "\0";
    sprintf(textValue, "%i", *value);
    
    Rectangle textBounds = { 0 };
    if (text != NULL)
    {
        textBounds.width = (float)GetTextWidth(text);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(VALUEBOX, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
        if (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(VALUEBOX, TEXT_PADDING);
    }
    
    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();
        
        bool valueHasChanged = false;
        
        if (editMode)
        {
            state = GUI_STATE_PRESSED;
            
            framesCounter++;
            
            int keyCount = (int)strlen(textValue);
            
            // Only allow keys in range [48..57]
            if (keyCount < VALUEBOX_MAX_CHARS)
            {
                if (GetTextWidth(textValue) < bounds.width)
                {
                    int key = GetCharPressed();
                    if ((key >= 48) && (key <= 57))
                    {
                        textValue[keyCount] = (char)key;
                        keyCount++;
                        valueHasChanged = true;
                    }
                }
            }
            
            // Delete text
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    keyCount--;
                    textValue[keyCount] = '\0';
                    framesCounter = 0;
                    if (keyCount < 0) keyCount = 0;
                    valueHasChanged = true;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                    if ((framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (framesCounter%2) == 0) keyCount--;
                    textValue[keyCount] = '\0';
                    if (keyCount < 0) keyCount = 0;
                    valueHasChanged = true;
                }
            }
            
            if (valueHasChanged) *value = TextToInteger(textValue);
            
            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) pressed = true;
        }
        else
        {
            if (*value > maxValue) *value = maxValue;
            else if (*value < minValue) *value = minValue;
            
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
            }
        }
        
        if (pressed) framesCounter = 0;
    }
    //--------------------------------------------------------------------
    
    // Draw control
    //--------------------------------------------------------------------
    Color baseColor = BLANK;
    if (state == GUI_STATE_PRESSED) baseColor = GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_PRESSED));
    else if (state == GUI_STATE_DISABLED) baseColor = GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_DISABLED));
    
    // WARNING: BLANK color does not work properly with Fade()
    GuiDrawRectangle(bounds, GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER + (state*3))), guiAlpha), baseColor);
    GuiDrawText(textValue, GetTextBounds(VALUEBOX, bounds), GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(VALUEBOX, TEXT + (state*3))), guiAlpha));
    
    // Draw blinking cursor
    if ((state == GUI_STATE_PRESSED) && (editMode && ((framesCounter/20)%2 == 0)))
    {
        // NOTE: ValueBox internal text is always centered
        Rectangle cursor = { bounds.x + GetTextWidth(textValue)/2 + bounds.width/2 + 2, bounds.y + 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), 1, bounds.height - 4*GuiGetStyle(VALUEBOX, BORDER_WIDTH) };
        GuiDrawRectangle(cursor, 0, BLANK, Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER_COLOR_PRESSED)), guiAlpha));
    }
    
    // Draw text label if provided
    if (text != NULL) GuiDrawText(text, textBounds, (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------
    
    return pressed;
}
