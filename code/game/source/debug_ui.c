#include "debug_ui.h"
#include "raylib.h"

typedef enum {
    DITEM_RAW,
    DITEM_TEXT,
    DITEM_BUTTON,
    DITEM_SLIDER,
    DITEM_LIST,
    DITEM_END,
    
    DITEM_FORCE_UINT8 = UINT8_MAX
} debug_kind;

typedef struct {
    float x, y;
} debug_draw_result;

#define DBG_FONT_SIZE 22
#define DBG_FONT_SPACING DBG_FONT_SIZE * 1.2f
#define DBG_START_XPOS 15
#define DBG_START_YPOS 200
#define DBG_LIST_XPOS_OFFSET 10
#define DBG_SHADOW_OFFSET_XPOS 1
#define DBG_SHADOW_OFFSET_YPOS 1
#define DBG_CTRL_HANDLE_DIM 10

static uint8_t is_shadow_rendered;
static uint8_t is_debug_open = 1;
static uint8_t is_handle_ctrl_held;
static float debug_xpos = DBG_START_XPOS;
static float debug_ypos = DBG_START_YPOS;

typedef struct debug_item {
    debug_kind kind;
    char const *name;
    float name_width;
    union {
        union {
            char const *text;
            uint64_t val;
        };
        
        struct {
            struct debug_item *items;
            uint8_t is_collapsed;
        } list;
        
        void (*on_click)(void);
    };
    
    debug_draw_result (*proc)(struct debug_item*, float, float);
} debug_item;

typedef enum {
    DAREA_OUTSIDE,
    DAREA_HOVER,
    DAREA_HELD,
    DAREA_PRESS,
    
    DAREA_FORCE_UINT8 = UINT8_MAX
} debug_area_status;

debug_area_status check_mouse_area(float xpos, float ypos, float w, float h);
bool is_btn_pressed(float xpos, float ypos, float w, float h, Color *color);

void UIDrawText(const char *text, float posX, float posY, int fontSize, Color color);
int UIMeasureText(const char *text, int fontSize);

#include "debug_ui_widgets.c"
#include "debug_ui_actions.c"

static debug_item items[] = {
    {
        .kind = DITEM_LIST, 
        .name = "general", 
        .list = {
            .items = (debug_item[]) {
                { .kind = DITEM_TEXT, .name = "delta time", .proc = DrawDeltaTime },
                { .kind = DITEM_TEXT, .name = "random literal", .text = "hello", .proc = DrawLiteral },
                { .kind = DITEM_TEXT, .name = "zoom", .proc = DrawZoom },
                { .kind = DITEM_END },
            }
        }
    },
    {
        .kind = DITEM_LIST,
        .name = "profilers",
        .list = {
            .items = (debug_item[]) {
                { .kind = DITEM_RAW, .val = PROF_MAIN_LOOP, .proc = DrawProfilerDelta },
                { .kind = DITEM_TEXT, .name = "unmeasured time", .proc = DrawUnmeasuredTime },
                { .kind = DITEM_RAW, .val = PROF_WORLD_WRITE, .proc = DrawProfilerDelta },
                { .kind = DITEM_RAW, .val = PROF_RENDER, .proc = DrawProfilerDelta },
                { .kind = DITEM_RAW, .val = PROF_UPDATE_SYSTEMS, .proc = DrawProfilerDelta },
                { .kind = DITEM_RAW, .val = PROF_ENTITY_LERP, .proc = DrawProfilerDelta },
                { .kind = DITEM_RAW, .val = PROF_ENTITY_REMOVAL, .proc = DrawProfilerDelta },
                { .kind = DITEM_END },
            },
            .is_collapsed = 1
        }
    },
    {
        .kind = DITEM_BUTTON,
        .name = "exit game",
        .on_click = ActExitGame,
    },
    {.kind = DITEM_END},
};

debug_draw_result debug_draw_list(debug_item *list, float xpos, float ypos, bool is_shadow) {
    is_shadow_rendered = is_shadow;
    for (debug_item *it = list; it->kind != DITEM_END; it += 1) {
        switch (it->kind) {
            case DITEM_LIST: {
                // NOTE(zaklaus): calculate and cache name width for future use
                if (it->name_width == 0) {
                    it->name_width = UIMeasureText(it->name, DBG_FONT_SIZE);
                }
                Color color = RAYWHITE;
                if (is_btn_pressed(xpos, ypos, it->name_width, DBG_FONT_SIZE, &color)) {
                    it->list.is_collapsed = !it->list.is_collapsed;
                }
                
                UIDrawText(it->name, xpos, ypos, DBG_FONT_SIZE, color);
                ypos += DBG_FONT_SPACING;
                if (it->list.is_collapsed) break;
                debug_draw_result res = debug_draw_list(it->list.items, xpos+DBG_LIST_XPOS_OFFSET, ypos, is_shadow);
                ypos = res.y;
            }break;
            
            case DITEM_TEXT: {
                char const *text = TextFormat("%s: ", it->name);
                if (it->name_width == 0) {
                    it->name_width = UIMeasureText(text, DBG_FONT_SIZE);
                }
                UIDrawText(text, xpos, ypos, DBG_FONT_SIZE, RAYWHITE);
                assert(it->proc);
                
                debug_draw_result res = it->proc(it, xpos + it->name_width, ypos);
                ypos = res.y;
            }break;
            
            case DITEM_RAW: {
                assert(it->proc);
                
                debug_draw_result res = it->proc(it, xpos, ypos);
                ypos = res.y;
            }break;
            
            case DITEM_BUTTON: {
                assert(it->on_click);
                char const *text = TextFormat("> %s", it->name);
                if (it->name_width == 0) {
                    it->name_width = UIMeasureText(text, DBG_FONT_SIZE);
                }
                Color color = RAYWHITE;
                if (is_btn_pressed(xpos, ypos, it->name_width, DBG_FONT_SIZE, &color)) {
                    it->on_click();
                }
                
                debug_draw_result res = DrawColoredText(xpos, ypos, text, color);
                ypos = res.y;
            }break;
            
            default: {
                
            }break;
        }
    }
    
    return (debug_draw_result){xpos, ypos};
}

void debug_draw(void) {
    float xpos = debug_xpos;
    float ypos = debug_ypos;
    
    // NOTE(zaklaus): move debug ui
    {
        debug_area_status area = check_mouse_area(xpos, ypos, DBG_CTRL_HANDLE_DIM, DBG_CTRL_HANDLE_DIM);
        Color color = BLUE;
        if (area == DAREA_HOVER) color = YELLOW;
        if (area == DAREA_HELD) {
            color = RED;
            is_handle_ctrl_held = 1;
        }
            
        if (is_handle_ctrl_held) {
            debug_xpos = xpos = GetMouseX() - DBG_CTRL_HANDLE_DIM/2;
            debug_ypos = ypos = GetMouseY() - DBG_CTRL_HANDLE_DIM/2;
                    
            if (area == DAREA_PRESS) {
                is_handle_ctrl_held = 0;
            }
        }
            
        DrawRectangle(xpos, ypos, DBG_CTRL_HANDLE_DIM, DBG_CTRL_HANDLE_DIM, color);
    }
    
    // NOTE(zaklaus): toggle debug ui
    {
        Color color = BLUE;
        debug_area_status area = check_mouse_area(xpos, 15+ypos, DBG_CTRL_HANDLE_DIM, DBG_CTRL_HANDLE_DIM);
        if (area == DAREA_HOVER) color = YELLOW;
        if (area == DAREA_HELD) {
            color = RED;
        }
        if (area == DAREA_PRESS) {
            is_debug_open = !is_debug_open;
        }
        DrawPoly((Vector2){xpos+DBG_CTRL_HANDLE_DIM/2, ypos+15+DBG_CTRL_HANDLE_DIM/2}, 3, 6.0f,is_debug_open ? 0.0f : 180.0f, color);
    }
    
    if (is_debug_open) {
        xpos += 15;
        debug_draw_list(items, xpos+DBG_SHADOW_OFFSET_XPOS, ypos+DBG_SHADOW_OFFSET_YPOS, 1); // NOTE(zaklaus): draw shadow
        debug_draw_list(items, xpos, ypos, 0);
    }
}

debug_area_status check_mouse_area(float xpos, float ypos, float w, float h) {
    if (is_shadow_rendered) return DAREA_OUTSIDE;
    bool is_inside = CheckCollisionPointRec(GetMousePosition(), (Rectangle){xpos, ypos, w, h});
    
    if (is_inside) {
        return IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ? DAREA_PRESS : IsMouseButtonDown(MOUSE_LEFT_BUTTON) ? DAREA_HELD : DAREA_HOVER;
    }
    return DAREA_OUTSIDE;
}

bool is_btn_pressed(float xpos, float ypos, float w, float h, Color *color) {
    assert(color);
    *color = RAYWHITE;
    debug_area_status area = check_mouse_area(xpos, ypos, w, h);
    if (area == DAREA_PRESS) {
        *color = RED;
        return true;
    } else if (area == DAREA_HOVER) {
        *color = YELLOW;
    } else if (area == DAREA_HELD) {
        *color = RED;
    }
    
    return false;
}

static inline 
void UIDrawText(const char *text, float posX, float posY, int fontSize, Color color) {
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        Vector2 position = { (float)posX , (float)posY  };
        
        int defaultFontSize = 10;   // Default Font chars height in pixel
        int new_spacing = fontSize/defaultFontSize;
        
        DrawTextEx(GetFontDefault(), text, position, (float)fontSize , (float)new_spacing , is_shadow_rendered ? BLACK : color);
    }
}

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
