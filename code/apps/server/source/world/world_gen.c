#include "zpl.h"

#include <math.h>
#include <stdlib.h>

#include "world/world.h"
#include "world/blocks.h"
#include "world/perlin.h"

#define WORLD_BLOCK_OBSERVER(name) uint32_t name(uint32_t id, uint32_t block_idx)
typedef WORLD_BLOCK_OBSERVER(world_block_observer_proc);

#define WORLD_PERLIN_FREQ    1.0
#define WORLD_PERLIN_OCTAVES 1

static void world_fill_rect(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, world_block_observer_proc *proc) {
    for (uint32_t cy=y; cy<y+h; cy++) {
        for (uint32_t cx=x; cx<x+w; cx++) {
            if (cx < 0 || cx >= world.width) continue;
            if (cy < 0 || cy >= world.height) continue;
            uint32_t i = (cy*world.width) + cx;

            if (proc) {
                uint32_t new_id = (*proc)(id, i);
                if (new_id != BLOCK_INVALID) {
                    id = new_id;
                }
                else continue;
            }

            world.data[i] = id;
        }
    }
}

static void world_fill_circle(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, world_block_observer_proc *proc) {
    for (uint32_t cy=y; cy<y+h; cy++) {
        for (uint32_t cx=x; cx<x+w; cx++) {
            if (cx < 0 || cx >= world.width) continue;
            if (cy < 0 || cy >= world.height) continue;
            uint32_t i = (cy*world.width) + cx;

            if (proc) {
                uint32_t new_id = (*proc)(id, i);
                if (new_id != BLOCK_INVALID) {
                    id = new_id;
                }
                else continue;
            }

            world.data[i] = id;
        }
    }
}

static void world_fill_rect_anchor(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, float ax, float ay, world_block_observer_proc *proc) {
    uint32_t w2 = (uint32_t)floorf(w*ax);
    uint32_t h2 = (uint32_t)floorf(h*ay);
    world_fill_rect(id, x-w2, y-h2, w, h, proc);
}

static WORLD_BLOCK_OBSERVER(shaper) {
    uint32_t biome = blocks_get_biome(id);
    uint32_t kind = blocks_get_kind(id);
    uint32_t old_biome = blocks_get_biome(world.data[block_idx]);
    uint32_t old_kind = blocks_get_kind(world.data[block_idx]);

    if (biome == old_biome) {
        if (kind == BLOCK_KIND_WALL && kind == old_kind) {
            return blocks_find(biome, BLOCK_KIND_HILL);
        }
        if (kind == BLOCK_KIND_HILL && kind == old_kind) {
            return blocks_find(biome, BLOCK_KIND_HILL_SNOW);
        }
    }

    return id;
}

static uint8_t world_perlin_cond(uint32_t block_idx, double chance) {
    uint32_t x = block_idx % world.width;
    uint32_t y = block_idx / world.width;

    return perlin_fbm(world.seed, x, y, WORLD_PERLIN_FREQ, WORLD_PERLIN_OCTAVES) < chance;
}

static WORLD_BLOCK_OBSERVER(shaper_noise80) {
    return world_perlin_cond(block_idx, 0.80) ? shaper(id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise50) {
    return world_perlin_cond(block_idx, 0.50) ? shaper(id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise33) {
    return world_perlin_cond(block_idx, 0.33) ? shaper(id, block_idx) : BLOCK_INVALID;
}

static void world_fill_mountain(uint32_t x, uint32_t y) {

}

#define RAND_RANGE(x,y) (x + (uint32_t)rand()%(y-(x)))

int32_t world_gen() {
    // TODO: perform world gen
    // atm, we will fill the world with ground and surround it by walls
    uint32_t wall_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WALL);
    uint32_t grnd_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_GROUND);
    uint32_t watr_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);

    srand(world.seed);

    // walls
    world_fill_rect(wall_id, 0, 0, world.width, world.height, NULL);

    // ground
    world_fill_rect(grnd_id, 1, 1, world.width-2, world.height-2, NULL);

    // water
    for (int i=0; i<RAND_RANGE(0, 12); i++) {
        world_fill_rect_anchor(watr_id, RAND_RANGE(0, world.width), RAND_RANGE(0, world.height), 4+RAND_RANGE(0,3), 4+RAND_RANGE(0,3), 0.5f, 0.5f, shaper_noise33);
    }

    const uint32_t HILLS_SIZE = 21;

    // hills
    for (int i=0; i<RAND_RANGE(8, 224); i++) {
        world_fill_rect_anchor(wall_id, RAND_RANGE(0, world.width), RAND_RANGE(0, world.height), RAND_RANGE(0,HILLS_SIZE), RAND_RANGE(0,HILLS_SIZE), 0.5f, 0.5f, shaper_noise33);
    }

    return WORLD_ERROR_NONE;
}
