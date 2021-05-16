static inline
void int_to_hex_color(uint32_t color, char *text);
static inline
int GuiDropdownBoxEco(Rectangle bounds, char const *text, char const *caption, int *active, bool editMode);

void texed_draw_topbar(zpl_aabb2 r) {
    zpl_aabb2 zoom_ctrl_r = zpl_aabb2_cut_left(&r, 150.0f);
    
    zoom = GuiSlider(aabb2_ray(zoom_ctrl_r), "zoom: ", zpl_bprintf("%.02f x", zoom), zoom, 1.0f, 16.0f);
    
    zpl_aabb2_cut_left(&r, 100.0f);
    
    zpl_aabb2 new_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    
    if (GuiButton(aabb2_ray(new_prj_r), "NEW")) {
        texed_destroy();
        texed_new(TD_DEFAULT_IMG_WIDTH, TD_DEFAULT_IMG_HEIGHT);
    }
    
    zpl_aabb2 load_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    static bool load_pending = false;
    
    if (GuiButton(aabb2_ray(load_prj_r), "LOAD")) {
        load_pending = true;
        ctx.fileDialog.fileDialogActive = true;
    }
    
    if (ctx.fileDialog.SelectFilePressed && load_pending) {
        ctx.fileDialog.SelectFilePressed = false;
        if (IsFileExtension(ctx.fileDialog.fileNameText, ".ecotex")) {
            zpl_strcpy(filename, ctx.fileDialog.fileNameText);
            ctx.filepath = filename;
            load_pending = false;
            texed_load();
        } else {
            ctx.fileDialog.fileDialogActive = true;
        }
    }
    
    zpl_aabb2 save_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    static bool save_as_pending = false;
    
    if (GuiButton(aabb2_ray(save_prj_r), "SAVE")) {
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
        if (!IsFileExtension(ctx.fileDialog.fileNameText, ".ecotex")) {
            zpl_strcpy(ctx.fileDialog.fileNameText, zpl_bprintf("%s.ecotex", ctx.fileDialog.fileNameText));
        }
        zpl_strcpy(filename, ctx.fileDialog.fileNameText);
        ctx.filepath = filename;
        save_as_pending = false;
        texed_save();
    }
    
    zpl_aabb2 prj_name_r = zpl_aabb2_cut_right(&r, 200.0f);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    GuiDrawText(zpl_bprintf("Project: %s", ctx.filepath ? ctx.filepath : "(unnamed)"), GetTextBounds(LABEL, aabb2_ray(prj_name_r)), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(BLACK, guiAlpha));
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
}

static bool is_add_op_dropbox_open = false;
static int add_op_dropbox_selected = 1;
static char add_op_list[2000] = {0};

void texed_draw_oplist_pane(zpl_aabb2 r) {
    zpl_aabb2 oplist_header = zpl_aabb2_cut_top(&r, 40.0f);
    
    zpl_aabb2 add_op_r = zpl_aabb2_cut_left(&oplist_header, 120.0f);
    
    if (!is_add_op_dropbox_open && GuiButton(aabb2_ray(add_op_r), "ADD OPERATION")) {
        is_add_op_dropbox_open = true;
        zpl_memset(add_op_list, 0, sizeof(add_op_list));
        
        for (int i = 0, cnt = 0; i < DEF_OPS_LEN; i += 1) {
            if (ctx.ops[i].is_locked) continue;
            zpl_strcat(add_op_list, zpl_bprintf("%.*s%s", cnt == 0 ? 0 : 1, ";", default_ops[i].name));
            cnt += 1;
        }
    }
    
    GuiSetState(ctx.filepath ? GUI_STATE_NORMAL : GUI_STATE_DISABLED);
    
    zpl_aabb2 export_code_r = zpl_aabb2_cut_left(&oplist_header, 120.0f);
    
    if (GuiButton(aabb2_ray(export_code_r), "BUILD TEXTURE")) {
        texed_export_cc(ctx.filepath);
    }
    
    zpl_aabb2 export_img_r = zpl_aabb2_cut_left(&oplist_header, 120.0f);
    
    if (GuiButton(aabb2_ray(export_img_r), "EXPORT AS IMAGE")) {
        texed_export_png(ctx.filepath);
    }
    
    GuiSetState(GUI_STATE_NORMAL);
    
    // NOTE(zaklaus): operator list
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        zpl_aabb2 op_item_r = zpl_aabb2_cut_top(&r, 22.5f);
        zpl_aabb2_cut_top(&op_item_r, 2.5f);
        zpl_aabb2_cut_bottom(&op_item_r, 2.5f);
        Rectangle list_item = aabb2_ray(op_item_r);
        DrawRectangleRec(list_item, ColorAlpha(ctx.selected_op == i ? GREEN : RED, 0.4f));
        
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
        
        zpl_aabb2 remove_r = zpl_aabb2_cut_right(&op_item_r, 60.0f);
        
        if (ctx.ops[i].is_locked) GuiSetState(GUI_STATE_DISABLED);
        if (GuiButton(aabb2_ray(remove_r), "REMOVE")) {
            texed_rem_op(i);
        }
        
        zpl_aabb2 hidden_r = zpl_aabb2_cut_right(&op_item_r, 60.0f);
        
        if (GuiButton(aabb2_ray(hidden_r), ctx.ops[i].is_hidden ? "SHOW" : "HIDE")) {
            ctx.ops[i].is_hidden = !ctx.ops[i].is_hidden;
            texed_repaint_preview();
        }
        GuiSetState(GUI_STATE_NORMAL);
        
        if (ctx.selected_op == i) GuiSetState(GUI_STATE_DISABLED);
        zpl_aabb2 select_r = zpl_aabb2_cut_right(&op_item_r, 60.0f);
        
        if (GuiButton(aabb2_ray(select_r), "SELECT")) {
            ctx.selected_op = i;
        }
        GuiSetState(GUI_STATE_NORMAL);
        
        GuiDrawText(zpl_bprintf("%s %s", ctx.ops[i].name, ctx.ops[i].is_locked ? "(locked)" : ""), GetTextBounds(LABEL, list_text), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(RAYWHITE, guiAlpha));
    }
    
    static int op_dropdown_state = 0;
    
    if (is_add_op_dropbox_open && (op_dropdown_state = GuiDropdownBoxEco(aabb2_ray(add_op_r), add_op_list, "ADD OPERATION", &add_op_dropbox_selected, true)) > 0) {
        is_add_op_dropbox_open = false;
        if (op_dropdown_state < 2) {
            texed_add_op(add_op_dropbox_selected);
        }
    }
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
    float prop_height = 40.0f;
    int prop_column_treshold = (int)zpl_floor(dims.height / prop_height);
    
    for (int i = 0; i < op->num_params; i += 1) {
        td_param *p = &op->params[i];
        zpl_aabb2 *c = (i >= prop_column_treshold) ? &column_2_r : &column_1_r;
        zpl_aabb2 item = zpl_aabb2_cut_top(c, prop_height);
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
                    zpl_aabb2 extra_r = zpl_aabb2_add_bottom(&tbox_r, prop_height);
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
            case TPARAM_COORD: {
                if (GuiValueBox(aabb2_ray(tbox_r), NULL, &p->i32, INT32_MIN, INT32_MAX, p->edit_mode)) {
                    p->edit_mode = !p->edit_mode;
                    
                    if (!p->edit_mode) {
                        sprintf(p->str, "%d", p->i32);
                        texed_repaint_preview();
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
