#include "world/blocks.h"

static block blocks[] = {
    {.name = "base-ground", .flags = 0, .kind = BLOCK_KIND_GROUND, .biome = 0, .symbol = '.', .drag = 1.0f, .friction = 1.0f },
    {.name = "base-dirt", .flags = 0, .kind = BLOCK_KIND_DIRT, .biome = 0, .symbol = ',', .drag = 2.1f , .friction = 1.0f },
    {.name = "base-wall", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_WALL, .biome = 0, .symbol = '#', .drag = 1.0f , .friction = 1.0f, .bounce = 2.0f },
    {.name = "base-hill", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_HILL, .biome = 0, .symbol = '^', .drag = 1.0f , .friction = 1.0f },
    {.name = "base-hill-snow", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_HILL_SNOW, .biome = 0, .symbol = '*', .drag = 1.0f , .friction = 1.0f },
    {.name = "base-water", .flags = 0, .kind = BLOCK_KIND_WATER, .biome = 0, .symbol = '~', .drag = 0.11f , .friction = 1.0f },
    {.name = "base-lava", .flags = BLOCK_FLAG_HAZARD, .kind = BLOCK_KIND_LAVA, .biome = 0, .symbol = '!', .drag = 6.2f , .friction = 4.0f },
};
