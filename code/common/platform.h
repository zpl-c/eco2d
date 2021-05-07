#pragma once
#include "system.h"

void platform_init();
void platform_shutdown();
uint8_t platform_is_running();

void platform_input();
void platform_render();
