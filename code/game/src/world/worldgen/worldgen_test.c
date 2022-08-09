#include "zpl.h"

#include <math.h>
#include <stdlib.h>

#include "world/world.h"
#include "world/blocks.h"
#include "world/perlin.h"

#include "modules/components.h"
#include "vehicle.h"
#include "items.h"
#include "world/blocks_info.h"

#define WORLD_BLOCK_OBSERVER(name) block_id name(block_id *data, block_id id, uint32_t block_idx)
typedef WORLD_BLOCK_OBSERVER(world_block_observer_proc);

#define WORLD_PERLIN_FREQ    100
#define WORLD_PERLIN_OCTAVES 1

#define BLOCK_INVALID 0xF

block_id worldgen_biome_find(uint32_t biome, uint32_t kind) {
    asset_id asset = ASSET_INVALID;
    switch (biome) {
        case BLOCK_BIOME_DEV: {
            switch (kind) {
                case BLOCK_KIND_GROUND: asset = ASSET_GROUND; break;
                case BLOCK_KIND_DIRT: asset = ASSET_DIRT; break;
                case BLOCK_KIND_WALL: asset = ASSET_WALL; break;
                case BLOCK_KIND_HILL_SNOW:
                case BLOCK_KIND_HILL: asset = ASSET_HILL; break;
                case BLOCK_KIND_WATER: asset = ASSET_WATER; break;
                case BLOCK_KIND_LAVA: asset = ASSET_LAVA; break;
            }
        }
    }
    
    return blocks_find(asset);
}

static world_data *world;

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

static void world_fill_circle(block_id *data, block_id id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, world_block_observer_proc *proc) {
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

static void world_fill_rect_anchor(block_id *data, block_id id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, float ax, float ay, world_block_observer_proc *proc) {
    uint32_t w2 = (uint32_t)floorf(w*ax);
    uint32_t h2 = (uint32_t)floorf(h*ay);
    world_fill_rect(data, id, x-w2, y-h2, w, h, proc);
}

static WORLD_BLOCK_OBSERVER(shaper) {
    uint32_t kind = id;
    uint32_t old_kind = data[block_idx];
    
    if (kind == BLOCK_KIND_WALL && kind == old_kind) {
        return worldgen_biome_find(BLOCK_BIOME_DEV, BLOCK_KIND_HILL);
    }
    if (kind == BLOCK_KIND_HILL && kind == old_kind) {
        return worldgen_biome_find(BLOCK_BIOME_DEV, BLOCK_KIND_HILL_SNOW);
    }
    
    return id;
}

static block_id world_perlin_cond_offset(uint32_t block_idx, double chance, uint32_t ofx, uint32_t ofy) {
    uint32_t x = block_idx % world->dim + ofx;
    uint32_t y = block_idx / world->dim + ofy;
    
    return perlin_fbm(world->seed, x, y, WORLD_PERLIN_FREQ, WORLD_PERLIN_OCTAVES) < chance;
}

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

#if 0
static void world_fill_mountain(uint32_t x, uint32_t y) {
    
}
#endif

#define RAND_RANGE(x,y) (x + (int)rand()%(y-(x)))
#define RAND_RANGEF(x,y) ((float)RAND_RANGE(x,y))

int32_t worldgen_test(world_data *wld) {
    // TODO(zaklaus): pass world as an arg instead
    world = wld;
    
    // TODO: perform world gen
    // atm, we will fill the world with ground and surround it by walls
    block_id wall_id = worldgen_biome_find(BLOCK_BIOME_DEV, BLOCK_KIND_WALL);
    block_id grnd_id = worldgen_biome_find(BLOCK_BIOME_DEV, BLOCK_KIND_GROUND);
    block_id dirt_id = worldgen_biome_find(BLOCK_BIOME_DEV, BLOCK_KIND_DIRT);
    block_id watr_id = worldgen_biome_find(BLOCK_BIOME_DEV, BLOCK_KIND_WATER);
    block_id lava_id = worldgen_biome_find(BLOCK_BIOME_DEV, BLOCK_KIND_LAVA);
    block_id tree_id = blocks_find(ASSET_TREE);
    
    srand(world->seed);
    
    // walls
    world_fill_rect(world->data, wall_id, 0, 0, world->dim, world->dim, NULL);
    
    // ground
    world_fill_rect(world->data, grnd_id, 1, 1, world->dim-2, world->dim-2, NULL);
    world_fill_rect(world->data, dirt_id, 1, 1, world->dim-2, world->dim-2, shaper_noise05);
    world_fill_rect(world->outer_data, tree_id, 1, 1, world->dim-2, world->dim-2, shaper_noise05b);
    
    // water
#if 1
    for (int i=0; i<RAND_RANGE(58, 92); i++) {
        world_fill_rect_anchor(world->data, watr_id, RAND_RANGE(0, world->dim), RAND_RANGE(0, world->dim), 4+RAND_RANGE(0,3), 4+RAND_RANGE(0,3), 0.5f, 0.5f, shaper_noise80);
    }
#endif
    
    // ice rink
#if 0
    world_fill_rect_anchor(world->data, watr_id, 450, 125, 10, 10, 0.0f, 0.0f, NULL);
#endif
    
    // lava
#if 1
    for (int i=0; i<RAND_RANGE(48, 62); i++) {
        world_fill_rect_anchor(world->data, lava_id, RAND_RANGE(0, world->dim), RAND_RANGE(0, world->dim), 4+RAND_RANGE(0,3), 4+RAND_RANGE(0,3), 0.5f, 0.5f, shaper_noise80);
    }
#endif
    
    
    // hills
#if 1
    const uint32_t HILLS_SIZE = 21;
    for (int i=0; i<RAND_RANGE(8, 124); i++) {
        world_fill_rect_anchor(world->data, wall_id, RAND_RANGE(0, world->dim), RAND_RANGE(0, world->dim), RAND_RANGE(0,HILLS_SIZE), RAND_RANGE(0,HILLS_SIZE), 0.5f, 0.5f, shaper_noise50);
    }
#endif
    
    // vehicles
#if 1
    for (int i=0; i<RAND_RANGE(258, 1124); i++) {
        uint64_t e = vehicle_spawn();
        
        Position *dest = ecs_get_mut(world_ecs(), e, Position);
        dest->x = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
        dest->y = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
    }
#endif
    
    // items
#if 1
    for (int i=0; i<RAND_RANGE(328, 164); i++) {
        uint64_t e = item_spawn(ASSET_DEMO_ICEMAKER, 32);
        
        Position *dest = ecs_get_mut(world_ecs(), e, Position);
        dest->x = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
        dest->y = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
    }
    
    for (int i=0; i<RAND_RANGE(328, 164); i++) {
        uint64_t e = item_spawn(ASSET_FENCE, 64);
        
        Position *dest = ecs_get_mut(world_ecs(), e, Position);
        dest->x = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
        dest->y = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
    }
    
    for (int i=0; i<RAND_RANGE(328, 164); i++) {
        uint64_t e = item_spawn(ASSET_WOOD, 64);
        
        Position *dest = ecs_get_mut(world_ecs(), e, Position);
        dest->x = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
        dest->y = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
    }
    
    for (int i=0; i<RAND_RANGE(128, 564); i++) {
        uint64_t e = item_spawn(ASSET_BELT, 999);
        
        Position *dest = ecs_get_mut(world_ecs(), e, Position);
        dest->x = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
        dest->y = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
    }
    
    for (int i=0; i<RAND_RANGE(128, 964); i++) {
        uint64_t e = item_spawn(ASSET_CHEST, 4);
        
        Position *dest = ecs_get_mut(world_ecs(), e, Position);
        dest->x = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
        dest->y = RAND_RANGEF(0, world->dim*WORLD_BLOCK_SIZE);
    }
    
#endif
    
    return WORLD_ERROR_NONE;
}
