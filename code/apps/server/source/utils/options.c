#include <stdio.h>

#include "world/world.h"
#include "world/blocks.h"
#include "utils/options.h"

void generate_minimap(int32_t seed, int32_t world_size) {
    world_init(seed, world_size, world_size);
    uint8_t const *world;
    uint32_t len = world_buf(&world, NULL);
    for (int i=0; i<len; i++) {
        if (i > 0 && i % world_size == 0) {
            putc('\n', stdout);
        }
        putc(blocks_get_symbol(world[i]), stdout);
    }
    putc('\n', stdout);
    world_destroy();
}
