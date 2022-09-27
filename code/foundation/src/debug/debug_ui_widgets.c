#include "debug/debug_ui.h"
#include "raylib.h"
#include "platform/platform.h"
#include "net/network.h"
#include "platform/profiler.h"

//~ NOTE(zaklaus): helpers

static inline debug_draw_result 
DrawFloat(float xpos, float ypos, float val) {
    char const *text = TextFormat("%.02f\n", val);
    UIDrawText(text, xpos, ypos, DBG_FONT_SIZE, RAYWHITE);
    return (debug_draw_result){.x = xpos + UIMeasureText(text, DBG_FONT_SIZE), .y = ypos + DBG_FONT_SPACING};
}

static inline debug_draw_result 
DrawColoredText(float xpos, float ypos, char const *text, Color color) {
    ZPL_ASSERT(text);
    UIDrawText(text, xpos, ypos, DBG_FONT_SIZE, color);
    
    char const *p = text;
    uint8_t newlines = 1;
    
    do {
        if (*p == '\n')
            ++newlines;
    } while (*p++ != 0);
    
    return (debug_draw_result){.x = xpos + UIMeasureText(text, DBG_FONT_SIZE), .y = ypos + DBG_FONT_SPACING*newlines};
}

static inline debug_draw_result 
DrawFormattedText(float xpos, float ypos, char const *text) {
    return DrawColoredText(xpos, ypos, text, RAYWHITE);
}

//~ NOTE(zaklaus): widgets

static inline debug_draw_result 
DrawCameraPos(debug_item *it, float xpos, float ypos) {
    (void)it;
    camera cam = camera_get();
    return DrawFormattedText(xpos, ypos, TextFormat("%d %d", (int)(cam.x/WORLD_BLOCK_SIZE), (int)(cam.y/WORLD_BLOCK_SIZE)));
}

static inline debug_draw_result 
DrawUnmeasuredTime(debug_item *it, float xpos, float ypos) {
    (void)it;
    float total_time = (float)profiler_delta(PROF_TOTAL_TIME);
    float acc_time = (float)profiler_delta(PROF_MAIN_LOOP);
    
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
    ZPL_ASSERT(it->text);
    return DrawFormattedText(xpos, ypos, it->text);
}

static inline debug_draw_result 
DrawProfilerDelta(debug_item *it, float xpos, float ypos) {
    float dt = (float)profiler_delta(it->val);
    return DrawFormattedText(xpos, ypos, TextFormat("%s: %.02f ms", profiler_name(it->val), dt * 1000.0f));
}

static inline debug_draw_result 
DrawReplaySamples(debug_item *it, float xpos, float ypos) {
    (void)it;
    size_t cnt = 0;
    if (records) {
        cnt = zpl_array_count(records);
    }
    return DrawFormattedText(xpos, ypos, TextFormat("%d of %d", record_pos, cnt));
}

static inline debug_draw_result 
DrawReplayFileName(debug_item *it, float xpos, float ypos) {
    (void)it;
    return DrawFormattedText(xpos, ypos, TextFormat("%s", replay_filename[0] ? replay_filename : "<unnamed>"));
}

// NOTE(zaklaus): demo npcs

static inline debug_draw_result 
DrawDemoNPCCount(debug_item *it, float xpos, float ypos) {
    (void)it;
    return DrawFormattedText(xpos, ypos, TextFormat("%d", demo_npcs ? zpl_array_count(demo_npcs) : 0));
}


// NOTE(zaklaus): world simulation 
static inline debug_draw_result 
DrawWorldStepSize(debug_item *it, float xpos, float ypos) {
    (void)it;
    return DrawFormattedText(xpos, ypos, TextFormat("%d ms", (int16_t)(sim_step_size*1000.f)));
}

// NOTE(zaklaus): network stats
static inline debug_draw_result 
DrawNetworkStats(debug_item *it, float xpos, float ypos) {
    (void)it;
    
    network_client_stats s = network_client_fetch_stats();
    
#define _kb(x) ((x) / 1024)
    
    debug_draw_result r;
    r = DrawFormattedText(xpos, ypos, TextFormat("recv total: %lld kb", _kb(s.total_received)));
    r = DrawFormattedText(xpos, r.y, TextFormat("sent total: %lld kb", _kb(s.total_sent)));
    
    r = DrawFormattedText(xpos, r.y, TextFormat("dn rate: %.02f kb/sec (%.02f kbit/sec)", _kb(s.incoming_bandwidth), _kb(s.incoming_bandwidth * 8.0f)));
    r = DrawFormattedText(xpos, r.y, TextFormat("up rate: %.02f kb/sec (%.02f kbit/sec)", _kb(s.outgoing_bandwidth), _kb(s.outgoing_bandwidth * 8.0f)));
    
    r = DrawFormattedText(xpos, r.y, TextFormat("packets sent: %lld", s.packets_sent));
    r = DrawFormattedText(xpos, r.y, TextFormat("packets lost: %d (%.02f%%)", s.packets_lost, s.packet_loss));
    
    r = DrawFormattedText(xpos, r.y, TextFormat("ping: %d ms", s.ping));
    
#undef _kb
    return r;
}
