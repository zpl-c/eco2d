#pragma once
#include "system.h"

void platform_init();
void platform_shutdown();
float platform_frametime();
uint8_t platform_is_running();

float platform_zoom_get(void);

void platform_input();
void platform_render();
