#include <stdio.h>

#include "world/world.h"
#include "world/blocks.h"
#include "utils/options.h"

void generate_minimap(int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size) {
    world_init(seed, block_size, chunk_size, world_size);

    uint8_t const *world;
    uint32_t world_length = chunk_size * world_size;
    uint32_t len = world_buf(&world, NULL);

    for (int i=0; i<len; i++) {
        if (i > 0 && i % world_length == 0) {
            putc('\n', stdout);
        }
        putc(blocks_get_symbol(world[i]), stdout);
    }

    putc('\n', stdout);
    world_destroy();
}
