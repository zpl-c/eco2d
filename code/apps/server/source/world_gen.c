#include "world.h"
#include "blocks.h"
#include "perlin.h"
#include "zpl.h"

#include <math.h>

#define WORLD_BLOCK_OBSERVER(name) uint32_t name(uint32_t id, uint32_t block_idx)
typedef WORLD_BLOCK_OBSERVER(world_block_observer_proc);

#define WORLD_PERLIN_FREQ    1.0
#define WORLD_PERLIN_OCTAVES 1

static void world_fill_rect(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, world_block_observer_proc *proc) {
    for (uint32_t cy=y; cy<y+h; cy++) {
        for (uint32_t cx=x; cx<x+w; cx++) {
            if (cx < 0 || cx >= world_width) continue;
            if (cy < 0 || cy >= world_height) continue;
            uint32_t i = (cy*world_width) + cx;

            if (proc) {
                uint32_t new_id = (*proc)(id, i);
                if (new_id != BLOCK_INVALID) {
                    id = new_id;
                }
                else continue;
            }

            world[i] = id;
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
    uint32_t old_biome = blocks_get_biome(world[block_idx]);
    uint32_t old_kind = blocks_get_kind(world[block_idx]);

    if (biome == old_biome) {
        if (kind == BLOCK_KIND_WALL && kind == old_kind) {
            return blocks_find(biome, BLOCK_KIND_HILL);
        }
    }

    return id;
}

static uint8_t world_perlin_cond(uint32_t block_idx, double chance) {
    uint32_t x = block_idx % world_width;
    uint32_t y = block_idx / world_width;

    return perlin_fbm(world_seed, x, y, WORLD_PERLIN_FREQ, WORLD_PERLIN_OCTAVES) < chance;
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

#define RAND_RANGE(x,y) (x + (uint32_t)rand()%(y-(x)))

int32_t world_gen() {
    // TODO: perform world gen
    // atm, we will fill the world with ground and surround it by walls
    uint32_t wall_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WALL);
    uint32_t grnd_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_GROUND);
    uint32_t watr_id = blocks_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);

    srand(world_seed);

    // walls
    world_fill_rect(wall_id, 0, 0, world_width, world_height, NULL);

    // ground
    world_fill_rect(grnd_id, 1, 1, world_width-2, world_height-2, NULL);

    // water
    for (int i=0; i<RAND_RANGE(0, 8); i++) {
        world_fill_rect_anchor(watr_id, RAND_RANGE(0, world_width), RAND_RANGE(0, world_height), 4+RAND_RANGE(0,3), 4+RAND_RANGE(0,3), 0.5f, 0.5f, shaper_noise33);
    }

    // hills
    world_fill_rect_anchor(wall_id, 14+RAND_RANGE(-10, 10), 21+RAND_RANGE(-10, 10), 8+RAND_RANGE(-2,4), 8+RAND_RANGE(-2,4), 0.5f, 0.5f, shaper_noise80);
    world_fill_rect_anchor(wall_id, 14+RAND_RANGE(-10, 10), 21+RAND_RANGE(-10, 10), 4+RAND_RANGE(-2,4), 4+RAND_RANGE(-2,4), 0.5f, 0.5f, shaper_noise50);

    for (int i=0; i<RAND_RANGE(8, 24); i++) {
        world_fill_rect_anchor(wall_id, RAND_RANGE(0, world_width), RAND_RANGE(0, world_height), 4+RAND_RANGE(-2,4), 4+RAND_RANGE(-2,4), 0.5f, 0.5f, shaper_noise50);
    }

    return WORLD_ERROR_NONE;
}
