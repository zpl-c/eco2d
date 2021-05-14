#include "gen/texgen.h"
#include "world/world.h"

Image texgen_build_block(uint32_t biome, uint32_t kind) {
    // TODO(zaklaus): 
    (void)biome;
    (void)kind;
    return GenImageWhiteNoise(WORLD_BLOCK_SIZE, WORLD_BLOCK_SIZE, 0.8f);
}

Texture2D texgen_build_sprite(asset_id id) {
    // TODO(zaklaus): 
    (void)id;
    Image img = GenImageColor(1, 1, RAYWHITE);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    
    return tex;
}
