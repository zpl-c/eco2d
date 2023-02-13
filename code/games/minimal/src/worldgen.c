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

int32_t worldgen_build(world_data *wld) {
    // TODO(zaklaus): pass world as an arg instead
    world = wld;

    // TODO: perform world gen
    // atm, we will fill the world with ground and surround it by walls
    block_id wall_id = blocks_find(ASSET_WALL);
    block_id grnd_id = blocks_find(ASSET_GROUND);
    block_id dirt_id = blocks_find(ASSET_DIRT);
    block_id watr_id = blocks_find(ASSET_WATER);
    block_id lava_id = blocks_find(ASSET_LAVA);
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
    world_fill_circle(world->data, grnd_id, world->dim / 2, world->dim / 2, (uint32_t)(radius * 0.7f), NULL);

    return WORLD_ERROR_NONE;
}
