#include "world/blocks.h"

#define BLOCK(a, f, s, ...)\
{\
.kind = a, .flags = f, .symbol = s, __VA_ARGS__\
}

static block blocks[] = {
    BLOCK(ASSET_EMPTY, 0, 'E'),
    BLOCK(ASSET_GROUND, 0, '.', .drag = 1.0f, .friction = 1.0f),
    BLOCK(ASSET_DIRT, 0, ',', .drag = 2.1f , .friction = 1.0f),
    BLOCK(ASSET_WALL, BLOCK_FLAG_COLLISION, '#', .drag = 1.0f , .friction = 1.0f, .bounce = 1.0f),
    BLOCK(ASSET_HILL, BLOCK_FLAG_COLLISION, '^', .drag = 1.0f , .friction = 1.0f),
    BLOCK(ASSET_HILL_SNOW, BLOCK_FLAG_COLLISION, '*', .drag = 1.0f , .friction = 1.0f),
    BLOCK(ASSET_WATER, 0, '~', .drag = 0.11f , .friction = 1.0f),
    BLOCK(ASSET_LAVA, BLOCK_FLAG_HAZARD, '!', .drag = 6.2f , .friction = 4.0f),
    BLOCK(ASSET_FENCE, BLOCK_FLAG_COLLISION, '#', .drag = 1.0f , .friction = 1.0f, .bounce = 1.0f),
    BLOCK(ASSET_WOOD, BLOCK_FLAG_COLLISION, '#', .drag = 1.0f , .friction = 1.0f, .bounce = 0.0f),
    BLOCK(ASSET_TREE, BLOCK_FLAG_COLLISION|BLOCK_FLAG_DESTROY_ON_COLLISION, '@', .drag = 1.0f , .friction = 1.0f, .bounce = 0.0f),

    BLOCK(ASSET_BELT_LEFT, 0, '@', .drag = 1.0f , .friction = 1.0f, .velx = -150.0f),
    BLOCK(ASSET_BELT_RIGHT, 0, '@', .drag = 1.0f , .friction = 1.0f, .velx = 150.0f),
    BLOCK(ASSET_BELT_UP, 0, '@', .drag = 1.0f , .friction = 1.0f, .vely = -150.0f),
    BLOCK(ASSET_BELT_DOWN, 0, '@', .drag = 1.0f , .friction = 1.0f, .vely = 150.0f),
};

ZPL_STATIC_ASSERT(sizeof(blocks)/sizeof(block) < ZPL_U16_MAX, "too many registered blocks! (max. 65536)");
