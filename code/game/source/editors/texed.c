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

#define TD_DEFAULT_IMG_WIDTH 64
#define TD_DEFAULT_IMG_HEIGHT 64
#define TD_UI_PADDING 5.0f
#define TD_UI_PREVIEW_BORDER 4.0f
#define TD_UI_DEFAULT_ZOOM 4.0f

static uint16_t screenWidth = 1280;
static uint16_t screenHeight = 720;
static float zoom = TD_UI_DEFAULT_ZOOM;
static float old_zoom = TD_UI_DEFAULT_ZOOM;
static Texture2D checker_tex;
static uint16_t old_screen_w;
static uint16_t old_screen_h;
static bool is_repaint_locked = false;

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
    bool is_locked;
    
    uint8_t num_params;
    td_param *params;
} td_op;

#define OP(n) .kind = n, .name = #n 

typedef struct {
    bool visible;
    char const *title;
    char const *message;
    char const *buttons;
    int result;
} td_msgbox;

typedef struct {
    char *filepath;
    Image img;
    Texture2D tex;
    GuiFileDialogState fileDialog;
    td_msgbox msgbox;
    bool is_saved;
    
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
void texed_export_cc(char const *path);
void texed_export_png(char const *path);
void texed_repaint_preview(void);
void texed_compose_image(void);
void texed_msgbox_init(char const *title, char const *message, char const *buttons);
void texed_process_ops(void);
void texed_process_params(void);
void texed_add_op(int idx);
void texed_rem_op(int idx);
void texed_swp_op(int idx, int idx2);

void texed_draw_oplist_pane(zpl_aabb2 r);
void texed_draw_props_pane(zpl_aabb2 r);
void texed_draw_topbar(zpl_aabb2 r);
void texed_draw_msgbox(zpl_aabb2 r);

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

void texed_run(int argc, char **argv) {
    zpl_opts opts={0};
    zpl_opts_init(&opts, zpl_heap(), argv[0]);
    zpl_opts_add(&opts, "td", "texed", "run texture editor", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "td-i", "texed-import", "convert an image to ecotex format", ZPL_OPTS_STRING);
    zpl_opts_add(&opts, "td-ec", "texed-export-cc", "export ecotex image to C header file", ZPL_OPTS_STRING);
    zpl_opts_add(&opts, "td-ep", "texed-export-png", "export ecotex image to PNG format", ZPL_OPTS_STRING);
    uint32_t ok = zpl_opts_compile(&opts, argc, argv);
    
    if (!ok) {
        zpl_opts_print_errors(&opts);
        zpl_opts_print_help(&opts);
        return;
    }
    
    if (zpl_opts_has_arg(&opts, "texed-import")) {
        zpl_string path = zpl_opts_string(&opts, "texed-import", "");
        if (FileExists(zpl_bprintf("art/%s", path)) && IsFileExtension(path, ".png")) {
            Image orig = LoadImage(zpl_bprintf("art/%s", path));
            texed_new(orig.width, orig.height);
            is_repaint_locked = true;
            texed_add_op(TOP_LOAD_IMAGE);
            td_param *params = ctx.ops[1].params;
            zpl_strcpy(params[0].str, path);
            is_repaint_locked = false;
            texed_compose_image();
            zpl_strcpy(filename, zpl_bprintf("%s.ecotex", GetFileNameWithoutExt(path)));
            ctx.filepath = filename;
            texed_save();
        } else {
            zpl_printf("%s\n", "provided file does not exist!");
        }
        return;
    }
    
    if (zpl_opts_has_arg(&opts, "texed-export-cc")) {
        zpl_string path = zpl_opts_string(&opts, "texed-export-cc", "");
        if (FileExists(zpl_bprintf("art/%s.ecotex", path))) {
            zpl_array_init(ctx.ops, zpl_heap());
            zpl_strcpy(filename, zpl_bprintf("%s.ecotex", path));
            ctx.filepath = filename;
            texed_load();
            texed_export_cc(path);
        } else {
            zpl_printf("%s\n", "provided file does not exist!");
        }
        return;
    }
    
    if (zpl_opts_has_arg(&opts, "texed-export-png")) {
        zpl_string path = zpl_opts_string(&opts, "texed-export-png", "");
        if (FileExists(zpl_bprintf("art/%s.ecotex", path))) {
            zpl_array_init(ctx.ops, zpl_heap());
            zpl_strcpy(filename, zpl_bprintf("%s.ecotex", path));
            ctx.filepath = filename;
            texed_load();
            texed_export_png(path);
        } else {
            zpl_printf("%s\n", "provided file does not exist!");
        }
        return;
    }
    
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
        zpl_aabb2 orig_screen = screen;
        
        zpl_aabb2 topbar = zpl_aabb2_cut_top(&screen, 20.0f);
        zpl_aabb2 oplist_pane = zpl_aabb2_cut_right(&screen, screenWidth / 2.0f);
        zpl_aabb2 property_pane = zpl_aabb2_cut_bottom(&screen, screenHeight / 2.0f);
        zpl_aabb2 preview_window = screen;
        
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
            if (ctx.msgbox.visible) GuiLock();
            
            DrawTextureEx(checker_tex, (Vector2){ preview_window.min.x, preview_window.min.y}, 0.0f, 1.0f, WHITE);
            DrawTextureEx(ctx.tex, (Vector2){ preview_window.min.x, preview_window.min.y}, 0.0f, zoom, WHITE);
            
            DrawAABB(topbar, RAYWHITE);
            DrawAABB(property_pane, GetColor(0x422060));
            DrawAABB(oplist_pane, GetColor(0x425060));
            
            texed_draw_topbar(topbar);
            texed_draw_props_pane(property_pane);
            texed_draw_oplist_pane(oplist_pane);
            
            if (ctx.fileDialog.fileDialogActive) GuiUnlock();
            if (ctx.msgbox.visible) GuiUnlock();
            
            GuiFileDialog(&ctx.fileDialog);
            texed_draw_msgbox(orig_screen);
        }
        EndDrawing();
    }
    
    UnloadTexture(checker_tex);
    zpl_opts_free(&opts);
    texed_destroy();
}

void texed_new(int32_t w, int32_t h) {
    ctx.img = GenImageColor(w, h, WHITE);
    ctx.filepath = NULL;
    ctx.selected_op = 0;
    ctx.msgbox.result = -1;
    zpl_array_init(ctx.ops, zpl_heap());
    
    is_repaint_locked = true;
    texed_add_op(TOP_NEW_IMAGE);
    zpl_i64_to_str(w, ctx.ops[0].params[0].str, 10);
    zpl_i64_to_str(h, ctx.ops[0].params[1].str, 10);
    is_repaint_locked = false;
    texed_repaint_preview();
    
    ctx.fileDialog = InitGuiFileDialog(420, 310, zpl_bprintf("%s/art", GetWorkingDirectory()), false);
    ctx.is_saved = true;
}

void texed_destroy(void) {
    UnloadTexture(ctx.tex);
    UnloadImage(ctx.img);
    zpl_array_free(ctx.ops);
}

void texed_export_cc(char const *path) {
    zpl_printf("Building texture %s.h ...\n", path);
    ExportImageAsCode(ctx.img, zpl_bprintf("art/gen/%s.h", GetFileNameWithoutExt(path)));
}

void texed_export_png(char const *path) {
    zpl_printf("Exporting texture %s.png ...\n", path);
    ExportImage(ctx.img, zpl_bprintf("art/gen/%s.png", GetFileNameWithoutExt(path)));
}

void texed_repaint_preview(void) {
    if (is_repaint_locked) return;
    texed_compose_image();
    
    if (!IsWindowReady()) return;
    UnloadTexture(ctx.tex);
    ctx.tex = LoadTextureFromImage(ctx.img);
}

void texed_compose_image(void) {
    if (is_repaint_locked) return;
    ctx.is_saved = false;
    texed_process_params();
    texed_process_ops();
}

void texed_msgbox_init(char const *title, char const *message, char const *buttons) {
    ctx.msgbox.result = -1;
    ctx.msgbox.visible = true;
    ctx.msgbox.title = title;
    ctx.msgbox.message = message;
    ctx.msgbox.buttons = buttons;
}

void texed_add_op(int idx) {
    assert(idx >= 0 && idx < DEF_OPS_LEN);
    td_op *dop = &default_ops[idx];
    
    td_op op = {
        .kind = dop->kind,
        .name = dop->name,
        .is_locked = dop->is_locked,
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
    if (idx == ctx.selected_op) ctx.selected_op = idx2;
    texed_repaint_preview();
}

void texed_rem_op(int idx) {
    assert(idx >= 0 && idx < (int)zpl_array_count(ctx.ops));
    zpl_mfree(ctx.ops[idx].params);
    zpl_array_remove_at(ctx.ops, idx);
    
    if (zpl_array_count(ctx.ops) > 0 && idx <= ctx.selected_op) ctx.selected_op -= 1;
    texed_repaint_preview();
}
