#include "zpl.h"
#include "world/world.h"

typedef struct {
    uint8_t *data;
    uint32_t seed;
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t chunk_width;
    uint16_t chunk_height;
    uint16_t chunk_amountx;
    uint16_t chunk_amounty;
} world_data;

static world_data world = {0};

int32_t world_gen();

int32_t world_init(int32_t seed, uint16_t chunk_width, uint16_t chunk_height, uint16_t chunk_amountx, uint16_t chunk_amounty) {
    if (world.data) {
        world_destroy();
    }
    world.seed = seed;
    world.width = chunk_width * chunk_amountx;
    world.height = chunk_height * chunk_amounty;
    world.size = world.width*world.height;
    world.data = zpl_malloc(sizeof(uint8_t)*world.size);

    if (!world.data) {
        return WORLD_ERROR_OUTOFMEM;
    }

    return world_gen();
}

int32_t world_destroy(void) {
    zpl_mfree(world.data);
    zpl_memset(&world, 0, sizeof(world));
    return WORLD_ERROR_NONE;
}

uint32_t world_buf(uint8_t const **ptr, uint32_t *width) {
    ZPL_ASSERT_NOT_NULL(world.data);
    ZPL_ASSERT_NOT_NULL(ptr);
    *ptr = world.data;
    if (width) *width = world.width;
    return world.size;
}

#include "world_gen.c"



// 11111111111111111111111111111111111111111111111111222222222222222222222222222222222222222222222333333333333333333333333333333333333333

// world 3x3
// chunk 3x3

// 111 111 111
// 222 222 222
// 333 333 333

// 111 111 111
// 222 222 222
// 333 333 333

// 111 111 111
// 222 222 222
// 333 333 333
