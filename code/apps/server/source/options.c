#include "options.h"
#include "world.h"
#include "blocks.h"
#include "zpl.h"

void generate_minimap(int32_t seed, int32_t world_size) {
    world_init(seed, world_size, world_size);
    uint8_t const *world;
    uint32_t len = world_buf(&world, NULL);
    for (int i=0; i<len; i++) {
        if (i > 0 && i % world_size == 0) {
            zpl_printf("\n");
        }
        zpl_printf("%c", blocks_get_symbol(world[i]));
    }
    zpl_printf("\n");
    world_destroy();
}
