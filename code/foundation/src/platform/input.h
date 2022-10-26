#pragma once
#include "system.h"

enum {
    IN_NONE,
    IN_LEFT,
    IN_RIGHT,
    IN_UP,
    IN_DOWN,
    IN_USE,
    IN_SPRINT,
    IN_DROP,
    IN_CTRL,
    IN_TOGGLE_INV,
    IN_TOGGLE_DEMOLITION,
};

enum {
    DEV_NONE,
    DEV_KEYBOARD,
    DEV_MOUSE,
    DEV_JOYSTICK
};

typedef struct {
    uint8_t device;
    uint32_t id;
} input_bind;

typedef struct {
    const char* name;
    uint8_t action;
    input_bind *binds;
} input_map;

uint8_t input_is_down(uint8_t action);
uint8_t input_is_pressed(uint8_t action);
uint8_t input_is_released(uint8_t action);

