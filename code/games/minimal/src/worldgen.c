#include "zpl.h"

#include <math.h>
#include <stdlib.h>

#include "world/world.h"
#include "world/blocks.h"
#include "world/perlin.h"

#include "ecs/components.h"
#include "ents/entity.h"
#include "ents/vehicle.h"
#include "ents/items.h"
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
    world_fill_rect(world->data, watr_id, 1, 1, world->dim-2, world->dim-2, NULL);

    int radius = 25;

    // wide boy circle
    world_fill_circle(world->data, dirt_id, world->dim / 2, world->dim / 2, radius, NULL);

    // narrow boy cirlce
    world_fill_circle(world->data, grnd_id, world->dim / 2, world->dim / 2, radius * 0.7f, NULL);


    return WORLD_ERROR_NONE;
}
