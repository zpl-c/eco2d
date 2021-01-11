#include "options.h"
#include "perlin.h"
#include "zpl.h"

#define TEST_MAP_DIM 32
#define TEST_MAP_DEPTH 18

static char *map_pattern = "~~..,,oo---OO^^^@@";

void generate_minimap(int32_t seed) {
    for (uint32_t y=0; y<TEST_MAP_DIM; y++) {
        for (uint32_t x=0; x<TEST_MAP_DIM; x++) {
            double sample = perlin_fbm(seed, x, y, 1.0, 1) * TEST_MAP_DEPTH;
            zpl_printf("%c", map_pattern[(uint32_t)sample]);
        }
        zpl_printf("\n");
    }
}
