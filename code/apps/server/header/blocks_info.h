#pragma once

typedef enum {
    BLOCK_KIND_DEV,
    BLOCK_KIND_GROUND,
    BLOCK_KIND_WATER,
    BLOCK_KIND_WALL,
    BLOCK_KIND_HILL,
    BLOCK_KIND_HOLE,
} block_kind;

typedef enum {
    BLOCK_BIOME_DEV,
    BLOCK_BIOME_PLAIN,
    BLOCK_BIOME_FOREST,
    BLOCK_BIOME_DESERT,
    BLOCK_BIOME_ICE,
    BLOCK_BIOME_OCEAN,
} block_biome;
