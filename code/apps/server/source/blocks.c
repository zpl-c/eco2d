#include "blocks.h"
#include "zpl.h"

// todo: csv parsing + utils

#define BLOCK_NAMELEN 80

typedef struct {
    uint8_t id;
    char name[BLOCK_NAMELEN];
    uint32_t flags;
} block;

static block *blocks = NULL;
static uint32_t blocks_count = 0;

int blocks_comparer(void const *a, void const *b) {
    block *ba = (block*)a;
    uint8_t bb = *(uint8_t*)b;
    return ba->id < bb ? -1 : ba->id > bb;
}

static block *blocks_find(uint8_t id) {
    ZPL_ASSERT_NOT_NULL(blocks);
    int32_t index = zpl_binary_search((void*)blocks, blocks_count, sizeof(block), (void*)&id, blocks_comparer);
    ZPL_ASSERT_MSG(index != -1, "block could not be found");
    return &blocks[index];
}

int32_t blocks_init(void) {
    // todo read csv by lines, linecount-1 == blocks_count
    // preallocate and assign values
    return BLOCKS_ERROR_NONE;
}

int32_t blocks_destroy(void) {
    ZPL_ASSERT_NOT_NULL(blocks);
    zpl_mfree(blocks);
    return BLOCKS_ERROR_NONE;
}

char *blocks_get_name(uint8_t id) {
    ZPL_ASSERT_NOT_NULL(blocks);
    return blocks_find(id)->name;
}

uint32_t blocks_get_flags(uint8_t id) {
    ZPL_ASSERT_NOT_NULL(blocks);
    return blocks_find(id)->flags;
}
