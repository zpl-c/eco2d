#include "debug_ui.h"
#include "debug_draw.h"
#include "raylib.h"
#include "vehicle.h"
#include "camera.h"
#include "world/world.h"
#include "game.h"
#include "sfd.h"

#include "modules/components.h"

typedef enum {
    DITEM_RAW,
    DITEM_GAP,
    DITEM_TEXT,
    DITEM_BUTTON,
    DITEM_SLIDER,
    DITEM_LIST,
    DITEM_COND,
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
#define DBG_GAP_HEIGHT DBG_FONT_SPACING * 0.5f

static uint8_t is_shadow_rendered;
static uint8_t is_debug_open = 1;
static uint8_t is_handle_ctrl_held;
static float debug_xpos = DBG_START_XPOS;
static float debug_ypos = DBG_START_YPOS;

typedef struct debug_item {
    debug_kind kind;
    char const *name;
    float name_width;
    uint8_t skip;
    
    union {
        union {
            char const *text;
            uint64_t val;
        };
        
        struct {
            struct debug_item *items;
            uint8_t is_collapsed;
        } list;
        
        struct {
            float val, min, max;
            void (*on_change)(float);
        } slider;
        
        void (*on_click)(void);
        
        uint8_t (*on_success)(void);
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

static void UIDrawText(const char *text, float posX, float posY, int fontSize, Color color);
static int UIMeasureText(const char *text, int fontSize);

#include "debug_replay.c"

#include "debug_ui_actions.c"
#include "debug_ui_widgets.c"

static debug_item items[] = {
    {
        .kind = DITEM_LIST, 
        .name = "general", 
        .list = {
            .items = (debug_item[]) {
                { .kind = DITEM_TEXT, .name = "delta time", .proc = DrawDeltaTime },
                { .kind = DITEM_TEXT, .name = "pos", .proc = DrawCameraPos },
                { .kind = DITEM_TEXT, .name = "zoom", .proc = DrawZoom },
                { .kind = DITEM_SLIDER, .name = "slider", .slider = { .min = 0.0f, .max = 1.0f, .val = 0.5f } },
                { .kind = DITEM_END },
            }
        }
    },
    {
        .kind = DITEM_LIST,
        .name = "debug actions",
        .list = {
            .items = (debug_item[]) {
                { .kind = DITEM_BUTTON, .name = "spawn car", .on_click = ActSpawnCar },
                { .kind = DITEM_BUTTON, .name = "place ice rink", .on_click = ActPlaceIceRink },
                { .kind = DITEM_BUTTON, .name = "spawn circling driver", .on_click = ActSpawnCirclingDriver },
                { .kind = DITEM_BUTTON, .name = "spawn icemaker item", .on_click = ActSpawnIcemaker },
                { 
                    .kind = DITEM_LIST,
                    .name = "demo npcs",
                    .list = {
                        .items = (debug_item[]) {
                            { .kind = DITEM_TEXT, .name = "npcs", .proc = DrawDemoNPCCount },
                            { .kind = DITEM_BUTTON, .name = "spawn 1000 npcs", .on_click = ActSpawnDemoNPCs },
                            { .kind = DITEM_BUTTON, .name = "remove all demo npcs", .on_click = ActDespawnDemoNPCs },
                            { .kind = DITEM_END },
                        },
                        .is_collapsed = true
                    }
                },
                { .kind = DITEM_END },
            }
        }
    },
    {
        .kind = DITEM_LIST,
        .name = "replay system",
        .list = {
            .items = (debug_item[]) {
                { .kind = DITEM_TEXT, .name = "macro", .proc = DrawReplayFileName },
                { .kind = DITEM_TEXT, .name = "samples", .proc = DrawReplaySamples },
                
                { .kind = DITEM_COND, .on_success = CondReplayDataPresentAndNotPlaying },
                { .kind = DITEM_BUTTON, .name = "replay", .on_click = ActReplayRun },
                
                { .kind = DITEM_COND, .on_success = CondReplayStatusOff },
                { .kind = DITEM_BUTTON, .name = "record", .on_click = ActReplayBegin },
                
                { .kind = DITEM_COND, .on_success = CondReplayStatusOn },
                { .kind = DITEM_BUTTON, .name = "stop", .on_click = ActReplayEnd },
                
                { .kind = DITEM_COND, .on_success = CondReplayIsPlaying },
                { .kind = DITEM_BUTTON, .name = "stop", .on_click = ActReplayEnd },
                
                { .kind = DITEM_COND, .on_success = CondReplayIsNotPlayingOrRecordsNotClear },
                { .kind = DITEM_BUTTON, .name = "clear", .on_click = ActReplayClear },
                
                { .kind = DITEM_GAP },
                
                { .kind = DITEM_COND, .on_success = CondReplayIsNotPlaying, .skip = 4 },
                { .kind = DITEM_BUTTON, .name = "new", .on_click = ActReplayNew },
                { .kind = DITEM_BUTTON, .name = "load", .on_click = ActReplayLoad },
                { .kind = DITEM_BUTTON, .name = "save", .on_click = ActReplaySave },
                { .kind = DITEM_BUTTON, .name = "save as...", .on_click = ActReplaySaveAs },
                
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
                { .kind = DITEM_RAW, .val = PROF_INTEGRATE_POS, .proc = DrawProfilerDelta },
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
            case DITEM_GAP: {
                ypos += DBG_GAP_HEIGHT;
            }break;
            case DITEM_COND: {
                ZPL_ASSERT(it->on_success);
                
                if (!it->on_success()) {
                    it += it->skip ? it->skip : 1;
                }
            }break;
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
                ZPL_ASSERT(it->proc);
                
                debug_draw_result res = it->proc(it, xpos + it->name_width, ypos);
                ypos = res.y;
            }break;
            
            case DITEM_RAW: {
                ZPL_ASSERT(it->proc);
                
                debug_draw_result res = it->proc(it, xpos, ypos);
                ypos = res.y;
            }break;
            
            case DITEM_BUTTON: {
                char const *text = TextFormat("> %s", it->name);
                if (it->name_width == 0) {
                    it->name_width = UIMeasureText(text, DBG_FONT_SIZE);
                }
                Color color = RAYWHITE;
                if (is_btn_pressed(xpos, ypos, it->name_width, DBG_FONT_SIZE, &color) && it->on_click) {
                    it->on_click();
                }
                
                if (!it->on_click) {
                    color = GRAY;
                }
                
                debug_draw_result res = DrawColoredText(xpos, ypos, text, color);
                ypos = res.y;
            }break;
            
            case DITEM_SLIDER: {
                ZPL_ASSERT(it->slider.min != it->slider.max);
                char const *text = TextFormat("%s: ", it->name);
                if (it->name_width == 0) {
                    it->name_width = UIMeasureText(text, DBG_FONT_SIZE);
                }
                UIDrawText(text, xpos, ypos, DBG_FONT_SIZE, RAYWHITE);
                xpos += it->name_width;
                
                DrawRectangleLines(xpos, ypos, 100.0f, DBG_FONT_SIZE, RAYWHITE);
                
                float stick_x = xpos + ((it->slider.val / it->slider.max) * 100.0f) - 5.0f;
                DrawRectangle(stick_x, ypos, 10.0f, DBG_FONT_SIZE, RED);
                
                xpos += 100.0f + 5.0f;
                DrawFloat(xpos, ypos, it->slider.val);
                ypos += DBG_FONT_SPACING;
            }break;
            
            default: {
                
            }break;
        }
    }
    
    return (debug_draw_result){xpos, ypos};
}

void debug_draw(void) {
    // NOTE(zaklaus): Flush old debug samples
    debug_draw_flush();
    
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
    ZPL_ASSERT(color);
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
