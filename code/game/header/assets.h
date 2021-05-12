#pragma once
#include "system.h"

typedef struct {
    uint16_t id;
    uint8_t kind;
    char* filename;
} asset_info;

enum {
    ASSET_KIND_IMAGE,
    ASSET_KIND_SOUND,
    ASSET_KIND_FORCE_8 = UINT8_MAX,
};

enum {
    ASSET_PLAYER,
    ASSET_FORCE_16 = UINT16_MAX,
};

extern asset_info assets[];
