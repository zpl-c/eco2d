#include "debug_ui.h"
#include "raylib.h"
#include "platform.h"
#include "profiler.h"

//~ NOTE(zaklaus): helpers

static inline debug_draw_result 
DrawFloat(float xpos, float ypos, float val) {
    char const *text = TextFormat("%.02f\n", val);
    UIDrawText(text, xpos, ypos, DBG_FONT_SIZE, RAYWHITE);
    return (debug_draw_result){.x = xpos + UIMeasureText(text, DBG_FONT_SIZE), .y = ypos + DBG_FONT_SPACING};
}

static inline debug_draw_result 
DrawFormattedText(float xpos, float ypos, char const *text) {
    assert(text);
    UIDrawText(text, xpos, ypos, DBG_FONT_SIZE, RAYWHITE);
    return (debug_draw_result){.x = xpos + UIMeasureText(text, DBG_FONT_SIZE), .y = ypos + DBG_FONT_SPACING};
}

static inline debug_draw_result 
DrawColoredText(float xpos, float ypos, char const *text, Color color) {
    assert(text);
    UIDrawText(text, xpos, ypos, DBG_FONT_SIZE, color);
    return (debug_draw_result){.x = xpos + UIMeasureText(text, DBG_FONT_SIZE), .y = ypos + DBG_FONT_SPACING};
}

//~ NOTE(zaklaus): widgets

static inline debug_draw_result 
DrawUnmeasuredTime(debug_item *it, float xpos, float ypos) {
    (void)it;
    float total_time = profiler_delta(PROF_TOTAL_TIME);
    float acc_time = profiler_delta(PROF_MAIN_LOOP);
    
    return DrawFormattedText(xpos, ypos, TextFormat("%.02f ms", (total_time-acc_time) * 1000.0f));
}

static inline debug_draw_result 
DrawDeltaTime(debug_item *it, float xpos, float ypos) {
    (void)it;
    float dt = GetFrameTime();
    return DrawFormattedText(xpos, ypos, TextFormat("%.02f (%.02f fps)", dt * 1000.0f, 1.0f/dt));
}

static inline debug_draw_result 
DrawZoom(debug_item *it, float xpos, float ypos) {
    (void)it;
    
    return DrawFloat(xpos, ypos, platform_zoom_get());
}

static inline debug_draw_result 
DrawLiteral(debug_item *it, float xpos, float ypos) {
    assert(it->text);
    return DrawFormattedText(xpos, ypos, it->text);
}

static inline debug_draw_result 
DrawProfilerDelta(debug_item *it, float xpos, float ypos) {
    float dt = profiler_delta(it->val);
    return DrawFormattedText(xpos, ypos, TextFormat("%s: %.02f ms", profiler_name(it->val), dt * 1000.0f));
}
