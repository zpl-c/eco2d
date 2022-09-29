#pragma once
#include "platform/system.h"
#include "world/blocks.h"
#include "world/world.h"
#include "world/perlin.h"

static world_data *world;

#define WORLD_BLOCK_OBSERVER(name) block_id name(block_id *data, block_id id, uint32_t block_idx)
typedef WORLD_BLOCK_OBSERVER(world_block_observer_proc);

#ifndef WORLD_CUSTOM_PERLIN
#define WORLD_PERLIN_FREQ    100
#define WORLD_PERLIN_OCTAVES 1
#endif

#define BLOCK_INVALID 0xF

// ensure it is set in worldgen_build

int worldgen_in_circle(int x, int y, int radius) {
    return (zpl_pow(x, 2) + zpl_pow(y, 2)) < zpl_pow(radius, 2);
}

static void world_fill_rect(block_id *data, block_id id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, world_block_observer_proc *proc) {
    for (uint32_t cy=y; cy<y+h; cy++) {
        for (uint32_t cx=x; cx<x+w; cx++) {
            if (cx < 0 || cx >= world->dim) continue;
            if (cy < 0 || cy >= world->dim) continue;
            uint32_t i = (cy*world->dim) + cx;

            if (proc) {
                block_id new_id = (*proc)(data, id, i);
                if (new_id != BLOCK_INVALID) {
                    id = new_id;
                }
                else continue;
            }

            data[i] = id;
        }
    }
}

static void world_fill_circle(block_id *data, block_id id, uint32_t cx, uint32_t cy, uint32_t radius, world_block_observer_proc *proc) {
    for (int x = -(int32_t)(radius); x < (int32_t)radius; ++x) {
        for (int y = -(int32_t)(radius); y < (int32_t)radius; ++y) {
            if (worldgen_in_circle(x, y, radius)) {
                int fx = x + cx;
                int fy = y + cy;

                uint32_t i = (fy*world->dim) + fx;

                if (proc) {
                    block_id new_id = (*proc)(data, id, i);
                    if (new_id != BLOCK_INVALID) {
                        id = new_id;
                    }
                    else continue;
                }

                data[i] = id;
            }
        }
    }
}

static void world_fill_rect_anchor(block_id *data, block_id id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, float ax, float ay, world_block_observer_proc *proc) {
    uint32_t w2 = (uint32_t)floorf(w*ax);
    uint32_t h2 = (uint32_t)floorf(h*ay);
    world_fill_rect(data, id, x-w2, y-h2, w, h, proc);
}


static block_id world_perlin_cond_offset(uint32_t block_idx, double chance, uint32_t ofx, uint32_t ofy) {
    uint32_t x = block_idx % world->dim + ofx;
    uint32_t y = block_idx / world->dim + ofy;

    return perlin_fbm(world->seed, x, y, WORLD_PERLIN_FREQ, WORLD_PERLIN_OCTAVES) < chance;
}

#ifndef WORLD_CUSTOM_SHAPER
static WORLD_BLOCK_OBSERVER(shaper) {
    uint32_t kind = id;

    return id;
}
#endif

static block_id world_perlin_cond(uint32_t block_idx, double chance) {
    return world_perlin_cond_offset(block_idx, chance, 0, 0);
}

#if 1
static WORLD_BLOCK_OBSERVER(shaper_noise80) {
    return world_perlin_cond(block_idx, 0.80) ? shaper(data, id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise50) {
    return world_perlin_cond(block_idx, 0.50) ? shaper(data, id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise33) {
    return world_perlin_cond(block_idx, 0.33) ? shaper(data, id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise05) {
    return world_perlin_cond(block_idx, 0.05) ? shaper(data, id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise05b) {
    return world_perlin_cond_offset(block_idx, 0.05, 32, 0) ? shaper(data, id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise01b) {
    return world_perlin_cond_offset(block_idx, 0.01, 32, 0) ? shaper(data, id, block_idx) : BLOCK_INVALID;
}
#else
static WORLD_BLOCK_OBSERVER(shaper_noise80) {
    return rand()%10 < 8 ? shaper(id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise50) {
    return rand()%10 < 5 ? shaper(id, block_idx) : BLOCK_INVALID;
}

static WORLD_BLOCK_OBSERVER(shaper_noise33) {
    return rand()%10 < 3 ? shaper(id, block_idx) : BLOCK_INVALID;
}
#endif


#define RAND_RANGE(x,y) (x + (int)rand()%(y-(x)))
#define RAND_RANGEF(x,y) ((float)RAND_RANGE(x,y))
