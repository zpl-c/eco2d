#include "world/blocks.h"

static block blocks[] = {
    {.name = "base-ground", .flags = 0, .kind = BLOCK_KIND_GROUND, .biome = 0, .symbol = '.'},
    {.name = "base-wall", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_WALL, .biome = 0, .symbol = '#'},
    {.name = "base-hill", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_HILL, .biome = 0, .symbol = '^'},
    {.name = "base-hill-snow", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_HILL_SNOW, .biome = 0, .symbol = '*'},
    {.name = "base-water", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_WATER, .biome = 0, .symbol = '~'},
};
