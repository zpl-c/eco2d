#pragma once

#define ASSET_INVALID 0xFF

#include "assets_ids.lst"

typedef enum {
#define X(idx) idx,
    _ASSETS
#undef X
    
    MAX_ASSETS
} asset_id;

extern const char *asset_names[];
