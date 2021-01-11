#include "blocks.h"

static block blocks[] = {
    {.tex_id = 0, .name = "base-ground", .flags = 0, .kind = BLOCK_KIND_GROUND, .biome = 0, .symbol = '.'},
    {.tex_id = 1, .name = "base-wall", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_WALL, .biome = 0, .symbol = '#'},
    {.tex_id = 2, .name = "base-water", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_WATER, .biome = 0, .symbol = '~'},
};
