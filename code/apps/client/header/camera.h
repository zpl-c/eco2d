#pragma once
#include "system.h"

typedef enum {
    CAMERA_MODE_STATIONARY,
    CAMERA_MODE_FOLLOW,
    CAMERA_MODE_FORCE_UINT8 = UINT8_MAX
} camera_mode;

typedef struct {
    camera_mode mode;
    uint64_t ent_id;
    double x, y;
} camera;

void camera_reset(void);
void camera_update(void);
void camera_set_follow(uint64_t ent_id);
void camera_set_pos(double x, double y);
camera camera_get(void);
