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

#define GUI_TEXTBOX_EXTENDED_IMPLEMENTATION
#include "gui_textbox_extended.h"

static uint16_t screenWidth = 1280;
static uint16_t screenHeight = 720;
static float zoom = 4.0f;
static Texture2D checker_tex;
static uint16_t old_screen_w;
static uint16_t old_screen_h;
static bool is_loading_prj = false;

#define TD_DEFAULT_IMG_WIDTH 64
#define TD_DEFAULT_IMG_HEIGHT 64
#define TD_UI_PADDING 5.0f
#define TD_UI_PREVIEW_BORDER 4.0f

typedef enum {
    TPARAM_FLOAT,
    TPARAM_COORD,
    TPARAM_INT,
    TPARAM_COLOR,
    TPARAM_STRING,
    
    TPARAM_FORCE_UINT8 = UINT8_MAX
} td_param_kind;

typedef struct {
    td_param_kind kind;
    char const *name;
    char str[1000];
    bool edit_mode;
    
    union {
        float flt;
        uint32_t u32;
        int32_t i32;
        Color color;
        char copy[4];
    };
} td_param;

typedef enum {
    TOP_NEW_IMAGE,
    TOP_DRAW_RECT,
    TOP_DRAW_LINE,
    TOP_DITHER,
    TOP_LOAD_IMAGE,
    TOP_DRAW_TEXT,
    TOP_RESIZE_IMAGE,
    
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
    int selected_op;
} td_ctx;

static td_ctx ctx = {0};

static char filename[200];

#include "texed_ops_list.c"

void texed_new(int32_t w, int32_t h);
void texed_destroy(void);
void texed_load(void);
void texed_save(void);
void texed_repaint_preview(void);
void texed_process_ops(void);
void texed_process_params(void);
void texed_add_op(int idx);
void texed_rem_op(int idx);
void texed_swp_op(int idx, int idx2);

void texed_draw_oplist_pane(zpl_aabb2 r);
void texed_draw_props_pane(zpl_aabb2 r);
void texed_draw_topbar(zpl_aabb2 r);

static inline
void DrawAABB(zpl_aabb2 rect, Color color) {
    DrawRectangleEco(rect.min.x, rect.min.y,
                     rect.max.x-rect.min.x,
                     rect.max.y-rect.min.y,
                     color);
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

#include "texed_ops.c"
#include "texed_prj.c"
#include "texed_widgets.c"

void texed_run(void) {
    InitWindow(screenWidth, screenHeight, "eco2d - texture editor");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    
    texed_new(TD_DEFAULT_IMG_WIDTH, TD_DEFAULT_IMG_HEIGHT);
    
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(RAYWHITE));
    
    while (!WindowShouldClose()) {
        zpl_aabb2 screen = {
            .min = (zpl_vec2) {.x = 0.0f, .y = 0.0f},
            .max = (zpl_vec2) {.x = GetScreenWidth(), .y = GetScreenHeight()},
        };
        
        zpl_aabb2 topbar = zpl_aabb2_cut_top(&screen, 25.0f);
        zpl_aabb2 oplist_pane = zpl_aabb2_cut_right(&screen, screenWidth / 2.0f);
        zpl_aabb2 preview_window = zpl_aabb2_cut_top(&screen, (screen.max.y-screen.min.y) / 2.0f);
        zpl_aabb2 property_pane = screen;
        
        // NOTE(zaklaus): contract all panes for a clean UI separation
        oplist_pane = zpl_aabb2_contract(&oplist_pane, TD_UI_PADDING);
        preview_window = zpl_aabb2_contract(&preview_window, TD_UI_PADDING);
        property_pane = zpl_aabb2_contract(&property_pane, TD_UI_PADDING);
        
        Rectangle preview_rect = aabb2_ray(preview_window);
        
        if (old_screen_w != GetScreenWidth() || old_screen_h != GetScreenHeight()) {
            old_screen_w = GetScreenWidth();
            old_screen_h = GetScreenHeight();
            Image checkerboard = GenImageChecked(preview_rect.width, preview_rect.height, 16, 16, BLACK, ColorAlpha(GRAY, 0.2f));
            checker_tex = LoadTextureFromImage(checkerboard);
            UnloadImage(checkerboard);
            ctx.fileDialog = InitGuiFileDialog(420, 310, zpl_bprintf("%s/art", GetWorkingDirectory()), false);
        }
        
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
            texed_draw_props_pane(property_pane);
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
    ctx.selected_op = 0;
    zpl_array_init(ctx.ops, zpl_heap());
    texed_add_op(TOP_NEW_IMAGE);
    
    ctx.fileDialog = InitGuiFileDialog(420, 310, zpl_bprintf("%s/art", GetWorkingDirectory()), false);
}

void texed_destroy(void) {
    UnloadTexture(ctx.tex);
    UnloadImage(ctx.img);
    zpl_array_free(ctx.ops);
}

void texed_repaint_preview(void) {
    if (is_loading_prj) return;
    UnloadTexture(ctx.tex);
    texed_process_params();
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
    ctx.selected_op = zpl_array_count(ctx.ops)-1;
    
    texed_repaint_preview();
}

void texed_swp_op(int idx, int idx2) {
    assert(idx >= 0 && idx < (int)zpl_array_count(ctx.ops));
    assert(idx2 >= 0 && idx2 < (int)zpl_array_count(ctx.ops));
    
    td_op tmp = ctx.ops[idx2];
    ctx.ops[idx2] = ctx.ops[idx];
    ctx.ops[idx] = tmp;
    ctx.selected_op = idx2;
    texed_repaint_preview();
}

void texed_rem_op(int idx) {
    assert(idx >= 0 && idx < (int)zpl_array_count(ctx.ops));
    zpl_mfree(ctx.ops[idx].params);
    zpl_array_remove_at(ctx.ops, idx);
    
    if (zpl_array_count(ctx.ops) > 0 && idx <= ctx.selected_op) ctx.selected_op -= 1;
    texed_repaint_preview();
}
