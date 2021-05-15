#define ZPL_NO_WINDOWS_H
#include "zpl.h"

#include "editors/texed.h"
#include "raylib.h"
#include "utils/raylib_helpers.h"
#include "cwpack/cwpack.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"

#define GUI_FILE_DIALOG_IMPLEMENTATION
#include "gui_file_dialog.h"

static uint16_t screenWidth = 1280;
static uint16_t screenHeight = 720;
static float zoom = 4.0f;

#define TD_DEFAULT_IMG_WIDTH 64
#define TD_DEFAULT_IMG_HEIGHT 64
#define TD_UI_PADDING 5.0f
#define TD_UI_PREVIEW_BORDER 4.0f

typedef enum {
    TPARAM_FLOAT,
    TPARAM_INT,
    TPARAM_COLOR,
    TPARAM_STRING,
    
    TPARAM_FORCE_UINT8 = UINT8_MAX
} td_param_kind;

typedef struct {
    td_param_kind kind;
    union {
        float number;
        int pix;
        char *str;
        Color color;
    };
} td_param;

typedef enum {
    TOP_CLEAR,
    TOP_DRAW_RECT,
    TOP_DRAW_LINE,
    
    TOP_FORCE_UINT8 = UINT8_MAX
} td_op_kind;

typedef struct {
    td_op_kind kind;
    char const *name;
    bool is_hidden;
    
    uint8_t num_params;
    td_param *params;
} td_op;

#define OP(n) .kind = n, .name = #n 

typedef struct {
    char *filepath;
    Image img;
    Texture2D tex;
    GuiFileDialogState fileDialog;
    
    td_op *ops; //< zpl_array
} td_ctx;

static td_ctx ctx = {0};

static char filename[200];

static td_op default_ops[] = {
    {
        OP(TOP_CLEAR),
        .num_params = 1,
        .params = (td_param[]) {
            {
                .kind = TPARAM_COLOR,
                .color = WHITE
            }
        }
    },
    {
        OP(TOP_DRAW_RECT),
        .num_params = 5,
        .params = (td_param[]) {
            {
                .kind = TPARAM_INT,
                .pix = 0
            },
            {
                .kind = TPARAM_INT,
                .pix = 0
            },
            {
                .kind = TPARAM_INT,
                .pix = 10
            },
            {
                .kind = TPARAM_INT,
                .pix = 10
            },
            {
                .kind = TPARAM_COLOR,
                .color = RED
            },
            
        }
    }
};

// NOTE(zaklaus): IMPORTANT !! keep these in sync
static char const *add_op_list = "CLEAR SOLID;DRAW RECTANGLE;PLOT LINE;DITHER";

#define DEF_OPS_LEN (int)(sizeof(default_ops) / (sizeof(default_ops[0])))

void texed_new(int32_t w, int32_t h);
void texed_destroy(void);
void texed_load(void);
void texed_save(void);
void texed_repaint_preview(void);
void texed_process_ops(void);
void texed_add_op(int idx);
void texed_rem_op(int idx);
void texed_swp_op(int idx, int idx2);

void texed_draw_oplist_pane(zpl_aabb2 r);
void texed_draw_topbar(zpl_aabb2 r);

static inline
void DrawAABB(zpl_aabb2 rect, Color color) {
    DrawRectangleEco(rect.min.x, rect.min.y,
                     rect.max.x-rect.min.x,
                     rect.max.y-rect.min.y,
                     color);
}

static inline
Rectangle aabb2_ray(zpl_aabb2 r);

void texed_run(void) {
    InitWindow(screenWidth, screenHeight, "eco2d - texture editor");
    SetTargetFPS(60);
    
    texed_new(TD_DEFAULT_IMG_WIDTH, TD_DEFAULT_IMG_HEIGHT);
    
    zpl_aabb2 screen = {
        .min = (zpl_vec2) {.x = 0.0f, .y = 0.0f},
        .max = (zpl_vec2) {.x = screenWidth, .y = screenHeight},
    };
    
    // TODO(zaklaus): TEMP
    texed_add_op(0);
    texed_add_op(1);
    
    zpl_aabb2 topbar = zpl_aabb2_cut_top(&screen, 25.0f);
    zpl_aabb2 oplist_pane = zpl_aabb2_cut_right(&screen, screenWidth / 2.0f);
    zpl_aabb2 preview_window = zpl_aabb2_cut_top(&screen, (screen.max.y-screen.min.y) / 2.0f);
    zpl_aabb2 property_pane = screen;
    
    // NOTE(zaklaus): contract all panes for a clean UI separation
    oplist_pane = zpl_aabb2_contract(&oplist_pane, TD_UI_PADDING);
    preview_window = zpl_aabb2_contract(&preview_window, TD_UI_PADDING);
    property_pane = zpl_aabb2_contract(&property_pane, TD_UI_PADDING);
    
    Rectangle preview_rect = aabb2_ray(preview_window);
    Image checkerboard = GenImageChecked(preview_rect.width, preview_rect.height, 16, 16, BLACK, ColorAlpha(GRAY, 0.2f));
    Texture2D checker_tex = LoadTextureFromImage(checkerboard);
    UnloadImage(checkerboard);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x222034));
        {
            if (ctx.fileDialog.fileDialogActive) GuiLock();
            DrawTextureEx(checker_tex, (Vector2){ preview_window.min.x, preview_window.min.y}, 0.0f, 1.0f, WHITE);
            DrawTextureEx(ctx.tex, (Vector2){ preview_window.min.x, preview_window.min.y}, 0.0f, zoom, WHITE);
            
            DrawAABB(topbar, RAYWHITE);
            DrawAABB(property_pane, GetColor(0x422060));
            DrawAABB(oplist_pane, GetColor(0x425060));
            
            texed_draw_topbar(topbar);
            texed_draw_oplist_pane(oplist_pane);
            
            if (ctx.fileDialog.fileDialogActive) GuiUnlock();
            GuiFileDialog(&ctx.fileDialog);
        }
        EndDrawing();
    }
    
    UnloadTexture(checker_tex);
    texed_destroy();
}

void texed_new(int32_t w, int32_t h) {
    ctx.img = GenImageColor(w, h, WHITE);
    ctx.filepath = NULL;
    zpl_array_init(ctx.ops, zpl_heap());
    texed_repaint_preview();
    
    ctx.fileDialog = InitGuiFileDialog(420, 310, zpl_bprintf("%s/art", GetWorkingDirectory()), false);
    //zpl_strcpy(ctx.fileDialog.filterExt, ".ecotex");
}

void texed_destroy(void) {
    UnloadTexture(ctx.tex);
    UnloadImage(ctx.img);
    zpl_array_free(ctx.ops);
}

void texed_repaint_preview(void) {
    UnloadTexture(ctx.tex);
    ImageClearBackground(&ctx.img, ColorAlpha(BLACK, 0.0f));
    texed_process_ops();
    ctx.tex = LoadTextureFromImage(ctx.img);
}

void texed_add_op(int idx) {
    assert(idx >= 0 && idx < DEF_OPS_LEN);
    td_op *dop = &default_ops[idx];
    
    td_op op = {
        .kind = dop->kind,
        .name = dop->name,
        .num_params = dop->num_params,
        .params = (td_param*)zpl_malloc(sizeof(td_param)*dop->num_params)
    };
    
    zpl_memcopy(op.params, dop->params, sizeof(td_param)*dop->num_params);
    
    zpl_array_append(ctx.ops, op);
    
    texed_repaint_preview();
}

void texed_rem_op(int idx) {
    assert(idx >= 0 && idx < (int)zpl_array_count(ctx.ops));
    zpl_mfree(ctx.ops[idx].params);
    zpl_array_remove_at(ctx.ops, idx);
    texed_repaint_preview();
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
        
    }
    
    zpl_aabb2 export_img_r = zpl_aabb2_cut_left(&oplist_header, 120.0f);
    
    if (GuiButton(aabb2_ray(export_img_r), "EXPORT AS IMAGE")) {
        
    }
    
    GuiSetState(GUI_STATE_NORMAL);
    
    // NOTE(zaklaus): operator list
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        zpl_aabb2 op_item_r = zpl_aabb2_cut_top(&r, 45.0f);
        zpl_aabb2_cut_top(&op_item_r, 2.5f);
        zpl_aabb2_cut_bottom(&op_item_r, 2.5f);
        Rectangle list_item = aabb2_ray(op_item_r);
        DrawRectangleRec(list_item, ColorAlpha(RED, 0.4f));
        
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
        
        GuiDrawText(ctx.ops[i].name, GetTextBounds(LABEL, list_text), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(RAYWHITE, guiAlpha));
    }
    
    if (is_add_op_dropbox_open && GuiDropdownBox(aabb2_ray(add_op_r), add_op_list, &add_op_dropbox_selected, true)) {
        is_add_op_dropbox_open = false;
        texed_add_op(add_op_dropbox_selected);
    }
}
static inline
Rectangle aabb2_ray(zpl_aabb2 r) {
    return (Rectangle) {
        .x = r.min.x,
        .y = r.min.y,
        .width = r.max.x-r.min.x,
        .height = r.max.y-r.min.y
    };
}

void texed_process_ops(void) {
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        td_op *op = &ctx.ops[i];
        if (op->is_hidden) continue;
        zpl_printf("processing op: %s ... \n", op->name);
        
        switch (op->kind) {
            case TOP_CLEAR: {
                ImageClearBackground(&ctx.img, op->params[0].color);
            }break;
            case TOP_DRAW_RECT: {
                ImageDrawRectangle(&ctx.img, 
                                   op->params[0].pix,
                                   op->params[1].pix,
                                   op->params[2].pix,
                                   op->params[3].pix,
                                   op->params[4].color);
            }break;
            default: {
                zpl_printf("%s\n", "unsupported op!");
            }break;
        }
    }
}

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
        zpl_strcpy(ctx.fileDialog.fileNameText, zpl_bprintf("%s.ecotex", ctx.fileDialog.fileNameText));
        zpl_strcpy(filename, ctx.fileDialog.fileNameText);
        ctx.filepath = filename;
        save_as_pending = false;
        texed_save();
    }
    
    zpl_aabb2 prj_name_r = zpl_aabb2_cut_right(&r, 200.0f);
    GuiDrawText(zpl_bprintf("Project: %s", ctx.filepath ? ctx.filepath : "(unnamed)"), GetTextBounds(LABEL, aabb2_ray(prj_name_r)), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(BLACK, guiAlpha));
}

void texed_swp_op(int idx, int idx2) {
    assert(idx >= 0 && idx < (int)zpl_array_count(ctx.ops));
    assert(idx2 >= 0 && idx2 < (int)zpl_array_count(ctx.ops));
    
    td_op tmp = ctx.ops[idx2];
    ctx.ops[idx2] = ctx.ops[idx];
    ctx.ops[idx] = tmp;
    
    texed_repaint_preview();
}

void texed_load(void) {
    // TODO(zaklaus): 
}

void texed_save(void) {
    assert(ctx.filepath);
    
    // TODO(zaklaus): 
}
