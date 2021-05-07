#include "platform.h"
#include "zpl.h"
#include <stdio.h>

uint8_t is_running;

void platform_init() {
    printf("eco2d-cli\n");
    is_running = 1;
}

void platform_shutdown() {
    printf("\nBye!\n");
    is_running = 0;
}

void platform_input() {
    
}

uint8_t platform_is_running() {
    return is_running;
}

void platform_render() {
    zpl_printf("o");
    zpl_sleep_ms(1000);
}
