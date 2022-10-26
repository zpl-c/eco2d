#include "input.h"
#include "raylib.h"

const static input_map maps[] = {
    {
        "left",
        IN_LEFT,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_LEFT},
            {DEV_KEYBOARD, KEY_A},
            {0}
        }
    },
    {
        "right",
        IN_RIGHT,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_RIGHT},
            {DEV_KEYBOARD, KEY_D},
            {0}
        }
    },
    {
        "up",
        IN_UP,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_UP},
            {DEV_KEYBOARD, KEY_W},
            {0}
        }
    },
    {
        "down",
        IN_DOWN,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_DOWN},
            {DEV_KEYBOARD, KEY_S},
            {0}
        }
    },
    {
        "use",
        IN_USE,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_SPACE},
            {0}
        }
    },
    {
        "sprint",
        IN_SPRINT,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_LEFT_SHIFT},
            {DEV_KEYBOARD, KEY_RIGHT_SHIFT},
            {0}
        }
    },
    {
        "drop",
        IN_DROP,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_G},
            {0}
        }
    },
    {
        "ctrl",
        IN_CTRL,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_LEFT_CONTROL},
            {DEV_KEYBOARD, KEY_RIGHT_CONTROL},
            {0}
        }
    },
    {
        "toggle inventory",
        IN_TOGGLE_INV,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_TAB},
            {0}
        }
    },
    {
        "toggle build",
        IN_TOGGLE_DEMOLITION,
        (input_bind[]){
            {DEV_KEYBOARD, KEY_B},
            {0}
        }
    },
    {0}
};

input_map input__get_map(uint8_t action) {
    for (const input_map *map = maps; map->action > 0; ++map) {
        if (map->action == action) {
            return *map;
        }
    }
    
    ZPL_PANIC("key not bound!");
    return maps[0];
}

uint8_t input_is_down(uint8_t action) {
    input_map map = input__get_map(action);
    for (input_bind *bind = map.binds; bind->device > 0; ++bind) {
        switch (bind->device) {
            case DEV_KEYBOARD: {
                if (IsKeyDown(bind->id)) return 1;
            }break;
            case DEV_MOUSE: {
                if (IsMouseButtonDown(bind->id)) return 1;
            }break;
            case DEV_JOYSTICK: {
                // TODO(zaklaus): 
            }break;
        }
    }
    
    return 0;
}

uint8_t input_is_pressed(uint8_t action) {
    input_map map = input__get_map(action);
    for (input_bind *bind = map.binds; bind->device > 0; ++bind) {
        switch (bind->device) {
            case DEV_KEYBOARD: {
                if (IsKeyPressed(bind->id)) return 1;
            }break;
            case DEV_MOUSE: {
                if (IsMouseButtonPressed(bind->id)) return 1;
            }break;
            case DEV_JOYSTICK: {
                // TODO(zaklaus): 
            }break;
        }
    }
    
    return 0;
}

uint8_t input_is_released(uint8_t action) {
    input_map map = input__get_map(action);
    for (input_bind *bind = map.binds; bind->device > 0; ++bind) {
        switch (bind->device) {
            case DEV_KEYBOARD: {
                if (IsKeyReleased(bind->id)) return 1;
            }break;
            case DEV_MOUSE: {
                if (IsMouseButtonReleased(bind->id)) return 1;
            }break;
            case DEV_JOYSTICK: {
                // TODO(zaklaus): 
            }break;
        }
    }
    
    return 0;
}
