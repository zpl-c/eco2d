#pragma once
#include "platform/system.h"

void platform_init(void);
void platform_shutdown(void);
void platform_request_close(void);
float platform_frametime(void);
uint8_t platform_is_running(void);
void platform_get_block_realpos(float *x, float *y);

float platform_zoom_get(void);

void platform_input(void);
void platform_render(void);
