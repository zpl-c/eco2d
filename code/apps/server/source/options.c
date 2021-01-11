#include "options.h"
#include "world.h"
#include "blocks.h"
#include "zpl.h"

#define TEST_MAP_DIM 32

void generate_minimap(int32_t seed) {
    world_init(seed, TEST_MAP_DIM, TEST_MAP_DIM);
    uint8_t *world;
    uint32_t len = world_buf(&world, NULL);
    for (int i=0; i<len; i++) {
        if (i > 0 && i % TEST_MAP_DIM == 0) {
            zpl_printf("\n");
        }
        zpl_printf("%c", blocks_get_symbol(world[i]));
    }
    zpl_printf("\n");
    world_destroy();
}
