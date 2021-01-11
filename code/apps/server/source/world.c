#include "world.h"
#include "zpl.h"

static uint8_t *world = NULL;
static uint32_t world_size = 0;

int32_t world_gen(int32_t seed, uint8_t width, uint8_t height) {
    return WORLD_ERROR_NONE;
}

int32_t world_destroy(void) {
    return WORLD_ERROR_NONE;
}

uint32_t world_buf(uint8_t const **ptr) {
    ZPL_ASSERT_NOT_NULL(world);
    ZPL_ASSERT_NOT_NULL(ptr);
    *ptr = world;
    return world_size;
}
