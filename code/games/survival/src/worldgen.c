#include "zpl.h"

#include <math.h>
#include <stdlib.h>

#include "world/world.h"
#include "world/blocks.h"
#include "world/perlin.h"

#include "models/components.h"
#include "models/entity.h"
#include "models/prefabs/vehicle.h"
#include "models/items.h"
#include "world/blocks_info.h"

#include "world/worldgen_utils.h"

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

#if 0
int32_t worldgen_build(world_data *wld) {
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
    world_fill_rect(world->data, wall_id, 1, 1, world->dim-2, world->dim-2, NULL);

    int radius = 90;

    // wide boy circle
    world_fill_circle(world->data, dirt_id, world->dim / 2, world->dim / 2, radius, NULL);

    // narrow boy cirlce
    world_fill_circle(world->data, grnd_id, world->dim / 2, world->dim / 2, (uint32_t)(radius * 0.7f), NULL);

	world_fill_circle(world->data, wall_id, world->dim / 2 + radius/3, world->dim / 2 + radius/3, (uint32_t)(radius * 0.2f), NULL);

    return WORLD_ERROR_NONE;
}
#else
int32_t worldgen_build(world_data *wld) {
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
    world_fill_rect(world->outer_data, tree_id, 1, 1, world->dim-2, world->dim-2, shaper_noise01b);

    // water
#if 1
    for (int i=0; i<RAND_RANGE(58, 92); i++) {
        world_fill_rect_anchor(world->outer_data, watr_id, RAND_RANGE(0, world->dim), RAND_RANGE(0, world->dim), 4+RAND_RANGE(0,3), 4+RAND_RANGE(0,3), 0.5f, 0.5f, shaper_noise80);
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

    return WORLD_ERROR_NONE;
}
#endif