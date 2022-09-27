#pragma once
#include "platform/system.h"
#include "raylib.h"

void debug_draw(void);

typedef enum {
    DAREA_OUTSIDE,
    DAREA_HOVER,
    DAREA_HELD,
    DAREA_PRESS,
    
    DAREA_FORCE_UINT8 = UINT8_MAX
} debug_area_status;

debug_area_status check_mouse_area(float xpos, float ypos, float w, float h);
bool is_btn_pressed(float xpos, float ypos, float w, float h, Color *color);
