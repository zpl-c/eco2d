#include "world.h"
#include "zpl.h"

typedef struct {
    uint8_t *data;
    uint32_t seed;
    uint32_t size;
    uint32_t width;
    uint32_t height;
} world_data;

static world_data world = {0};

int32_t world_gen();

int32_t world_init(int32_t seed, uint32_t width, uint32_t height) {
    if (world.data) {
        world_destroy();
    }
    world.seed = seed;
    world.width = width;
    world.height = height;
    world.size = width*height;
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
