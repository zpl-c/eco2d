#include "world.h"
#include "blocks.h"
#include "zpl.h"

static void world_fill(uint8_t *world, uint32_t width, uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    for (uint32_t cy=y; cy<y+h; cy++) {
        for (uint32_t cx=x; cx<x+w; cx++) {
            uint32_t i = (cy*width) + cx;
            world[i] = id;
        }
    }
}

int32_t world_gen(uint8_t *world, uint32_t size, uint32_t width, uint32_t height, int32_t seed) {
    // TODO: perform world gen
    // atm, we will fill the world with ground and surround it by walls
    uint32_t wall_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WALL);
    uint32_t grnd_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_GROUND);
    uint32_t watr_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);

    // walls
    world_fill(world, width, wall_id, 0, 0, width, height);

    // ground
    world_fill(world, width, grnd_id, 1, 1, width-2, height-2);

    // water
    world_fill(world, width, watr_id, 5, 5, 3, 3);

    return WORLD_ERROR_NONE;
}
