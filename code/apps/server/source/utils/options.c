#include <stdio.h>

#include "world/world.h"
#include "world/blocks.h"
#include "utils/options.h"

void generate_minimap(int32_t seed, uint16_t chunk_size, uint16_t chunk_amount) {
    world_init(seed, chunk_size, chunk_size, chunk_amount, chunk_amount);

    uint8_t const *world;
    uint32_t world_size = chunk_size * chunk_amount;
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
