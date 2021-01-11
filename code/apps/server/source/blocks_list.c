#include "blocks.h"
#include "atlas_shared.h"

static block blocks[] = {
    {.tex_id = ATLAS_XY(0, 0), .name = "base-ground", .flags = 0, .kind = BLOCK_KIND_GROUND, .biome = 0, .symbol = '.'},
    {.tex_id = ATLAS_XY(1, 0), .name = "base-wall", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_WALL, .biome = 0, .symbol = '#'},
    {.tex_id = ATLAS_XY(0, 1), .name = "base-water", .flags = BLOCK_FLAG_COLLISION, .kind = BLOCK_KIND_WATER, .biome = 0, .symbol = '~'},
};
