static inline
void int_to_hex_color(uint32_t color, char *text);

void texed_draw_topbar(zpl_aabb2 r) {
    zpl_aabb2 zoom_ctrl_r = zpl_aabb2_cut_left(&r, 150.0f);
    
    zoom = GuiSlider(aabb2_ray(zoom_ctrl_r), "zoom: ", zpl_bprintf("%.02f x", zoom), zoom, 1.0f, 16.0f);
    
    zpl_aabb2_cut_left(&r, 100.0f);
    
    zpl_aabb2 new_prj_r = zpl_aabb2_cut_left(&r, 60.0f);
    
    if (GuiButton(aabb2_ray(new_prj_r), "NEW")) {
        texed_destroy();
        texed_new(TD_DEFAULT_IMG_WIDTH, TD_DEFAULT_IMG_HEIGHT); // TODO(zaklaus): show res panel
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
static int add_op_dropbox_selected = 0;

void texed_draw_oplist_pane(zpl_aabb2 r) {
    zpl_aabb2 oplist_header = zpl_aabb2_cut_top(&r, 40.0f);
    
    zpl_aabb2 add_op_r = zpl_aabb2_cut_left(&oplist_header, 120.0f);
    
    if (!is_add_op_dropbox_open && GuiButton(aabb2_ray(add_op_r), "ADD OPERATION")) {
        is_add_op_dropbox_open = true;
    }
    
    
    GuiSetState(ctx.filepath ? GUI_STATE_NORMAL : GUI_STATE_DISABLED);
    
    zpl_aabb2 export_code_r = zpl_aabb2_cut_left(&oplist_header, 120.0f);
    
    if (GuiButton(aabb2_ray(export_code_r), "BUILD TEXTURE")) {
        zpl_printf("Building texture %s.h ...\n", ctx.filepath);
        ExportImageAsCode(ctx.img, zpl_bprintf("art/gen/%s.h", ctx.filepath));
    }
    
    zpl_aabb2 export_img_r = zpl_aabb2_cut_left(&oplist_header, 120.0f);
    
    if (GuiButton(aabb2_ray(export_img_r), "EXPORT AS IMAGE")) {
        zpl_printf("Exporting texture %s.png ...\n", ctx.filepath);
        ExportImage(ctx.img, zpl_bprintf("art/gen/%s.png", ctx.filepath));
    }
    
    GuiSetState(GUI_STATE_NORMAL);
    
    // NOTE(zaklaus): operator list
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        zpl_aabb2 op_item_r = zpl_aabb2_cut_top(&r, 45.0f);
        zpl_aabb2_cut_top(&op_item_r, 2.5f);
        zpl_aabb2_cut_bottom(&op_item_r, 2.5f);
        Rectangle list_item = aabb2_ray(op_item_r);
        DrawRectangleRec(list_item, ColorAlpha(ctx.selected_op == i ? GREEN : RED, 0.4f));
        
        zpl_aabb2 swap_r = zpl_aabb2_cut_left(&op_item_r, 50.0f);
        Rectangle list_text = aabb2_ray(op_item_r);
        
        zpl_aabb2_cut_right(&swap_r, 5.0f);
        zpl_aabb2 swap_top = zpl_aabb2_cut_top(&swap_r, 20.0f);
        zpl_aabb2 swap_bottom = swap_r;
        
        if (i > 0 && GuiButton(aabb2_ray(swap_top), "UP")) {
            texed_swp_op(i, i-1);
        }
        
        if (i+1 < zpl_array_count(ctx.ops) && GuiButton(aabb2_ray(swap_bottom), "DOWN")) {
            texed_swp_op(i, i+1);
        }
        
        zpl_aabb2 remove_r = zpl_aabb2_cut_right(&op_item_r, 60.0f);
        
        if (GuiButton(aabb2_ray(remove_r), "REMOVE")) {
            texed_rem_op(i);
        }
        
        zpl_aabb2 hidden_r = zpl_aabb2_cut_right(&op_item_r, 60.0f);
        
        if (GuiButton(aabb2_ray(hidden_r), ctx.ops[i].is_hidden ? "SHOW" : "HIDE")) {
            ctx.ops[i].is_hidden = !ctx.ops[i].is_hidden;
            texed_repaint_preview();
        }
        
        if (ctx.selected_op != i) {
            zpl_aabb2 select_r = zpl_aabb2_cut_right(&op_item_r, 60.0f);
            
            if (GuiButton(aabb2_ray(select_r), "SELECT")) {
                ctx.selected_op = i;
            }
        }
        
        GuiDrawText(ctx.ops[i].name, GetTextBounds(LABEL, list_text), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(RAYWHITE, guiAlpha));
    }
    
    if (is_add_op_dropbox_open && GuiDropdownBox(aabb2_ray(add_op_r), add_op_list, &add_op_dropbox_selected, true)) {
        is_add_op_dropbox_open = false;
        texed_add_op(add_op_dropbox_selected);
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
        
        switch (p->kind) {
            case TPARAM_COLOR: {
                if (!p->edit_mode && GuiTextBox(aabb2_ray(tbox_r), p->str, 64, p->edit_mode)) {
                    p->edit_mode = true;
                }
                
                if (p->edit_mode) {
                    zpl_aabb2 ok_r = zpl_aabb2_cut_left(&tbox_r, 50.0f);
                    p->color = GuiColorPicker(aabb2_ray(tbox_r), p->color);
                    
                    if (GuiButton(aabb2_ray(ok_r), "OK")) {
                        p->edit_mode = false;
                        int_to_hex_color(ColorToInt(p->color), p->str);
                        texed_repaint_preview();
                    }
                }
            }break;
            
            default: {
                if (GuiTextBox(aabb2_ray(tbox_r), p->str, 64, p->edit_mode)) {
                    p->edit_mode = !p->edit_mode;
                    
                    if (!p->edit_mode)
                        texed_repaint_preview();
                }
            }break;
        };
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
