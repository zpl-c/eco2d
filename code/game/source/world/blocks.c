#define ZPL_PICO
#include "zpl.h"
#include "world/blocks.h"
#include "raylib.h"

#define BLOCKS_COUNT (sizeof(blocks)/sizeof(block))

typedef struct {
    char *name;
    uint32_t flags;
    uint32_t kind;
    uint32_t biome;
    char symbol;
    
    // NOTE(zaklaus): viewer data
    Image tex;
} block;

#include "blocks_list.c"

uint8_t blocks_find(uint32_t biome, uint32_t kind) {
    for (uint32_t i=0; i<BLOCKS_COUNT; i++) {
        if (blocks[i].biome == biome && blocks[i].kind == kind)
            return i;
    }
    return BLOCK_INVALID;
}

char *blocks_get_name(uint8_t id) {
    return blocks[id].name;
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
