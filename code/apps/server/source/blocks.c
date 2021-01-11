#include "blocks.h"
#include "zpl.h"

// todo: csv parsing + utils

#define BLOCK_NAMELEN 80
#define BLOCKS_COUNT (sizeof(blocks)/sizeof(block))

typedef struct {
    uint8_t tex_id;
    char name[BLOCK_NAMELEN];
    uint32_t flags;
    uint32_t kind;
    uint32_t biome;
    char symbol;
} block;

#include "blocks_list.c"

uint8_t blocks_find(uint32_t biome, uint32_t kind) {
    for (int i=0; i<BLOCKS_COUNT; i++) {
        if (blocks[i].biome == biome && blocks[i].kind == kind)
            return i;
    }
    return BLOCK_INVALID;
}

char *blocks_get_name(uint8_t id) {
    return blocks[id].name;
}

uint8_t blocks_get_tex_id(uint8_t id) {
    return blocks[id].tex_id;
}

char blocks_get_symbol(uint8_t id) {
    return blocks[id].symbol;
}

uint32_t blocks_get_flags(uint8_t id) {
    return blocks[id].flags;
}

uint32_t blocks_get_biome(uint8_t id) {
    return blocks[id].biome;
}

uint32_t blocks_get_kind(uint8_t id) {
    return blocks[id].kind;
}
