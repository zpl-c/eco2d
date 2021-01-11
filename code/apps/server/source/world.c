#include "world.h"
#include "zpl.h"

static uint8_t *world = NULL;
static uint32_t world_seed = 0;
static uint32_t world_size = 0;
static uint32_t world_width = 0;
static uint32_t world_height = 0;

int32_t world_gen();

int32_t world_init(int32_t seed, uint32_t width, uint32_t height) {
    if (world) {
        world_destroy();
    }
    world_seed = seed;
    world_width = width;
    world_height = height;
    world_size = width*height;
    world = zpl_malloc(sizeof(uint8_t)*world_size);

    if (!world) {
        return WORLD_ERROR_OUTOFMEM;
    }
    return world_gen();
}

int32_t world_destroy(void) {
    zpl_mfree(world);
    world = NULL;
    return WORLD_ERROR_NONE;
}

uint32_t world_buf(uint8_t const **ptr, uint32_t *width) {
    ZPL_ASSERT_NOT_NULL(world);
    ZPL_ASSERT_NOT_NULL(ptr);
    *ptr = world;
    if (width) *width = world_width;
    return world_size;
}

#include "world_gen.c"