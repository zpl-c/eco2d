#include "world.h"
#include "blocks.h"
#include "zpl.h"

#include <math.h>

static void world_fill_rect(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    for (uint32_t cy=y; cy<y+h; cy++) {
        for (uint32_t cx=x; cx<x+w; cx++) {
            uint32_t i = (cy*world_width) + cx;
            world[i] = id;
        }
    }
}

static void world_fill_dot(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    uint32_t w2 = (uint32_t)floor(w/2.0);
    uint32_t h2 = (uint32_t)floor(h/2.0);
    world_fill_rect(id, x-w2, y-h2, w, h);
}

int32_t world_gen(int32_t seed) {
    // TODO: perform world gen
    // atm, we will fill the world with ground and surround it by walls
    uint32_t wall_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WALL);
    uint32_t grnd_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_GROUND);
    uint32_t watr_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);

    // walls
    world_fill_rect(wall_id, 0, 0, world_width, world_height);

    // ground
    world_fill_rect(grnd_id, 1, 1, world_width-2, world_height-2);

    // water
    world_fill_dot(watr_id, 8, 8, 4, 4);

    return WORLD_ERROR_NONE;
}
