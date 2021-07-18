#include "gen/texgen.h"
#include "world/world.h"
#include "texgen_data.c"

#define ZPL_NANO
#include "zpl.h"

static inline
Image LoadImageEco(const char *name) {
    static char filename[128];
    zpl_snprintf(filename,  128, "art/gen/%s.png", name);
    return LoadImage(filename);
}

Image texgen_build_block(uint32_t biome, uint32_t kind) {
    // TODO(zaklaus): 
    
    switch (biome) {
        case BLOCK_BIOME_DEV: {
            switch (kind) {
                case BLOCK_KIND_GROUND:{
                    return LoadImageEco("grass");
                }break;
                case BLOCK_KIND_DIRT:{
                    return LoadImageEco("dirt");
                }break;
                case BLOCK_KIND_WALL:{
                    return LoadImageEco("asphalt");
                }break;
                case BLOCK_KIND_HILL_SNOW:
                case BLOCK_KIND_HILL:{
                    return LoadImageEco("rock");
                }break;
            }
        }
    }
    
    return GenImageColor(WORLD_BLOCK_SIZE,WORLD_BLOCK_SIZE,ColorFromHSV(biome+kind*30, 0.13f, 0.89f));
}

Texture2D texgen_build_sprite(asset_id id) {
    // TODO(zaklaus): 
    (void)id;
    Image img = GenImageColor(1, 1, RAYWHITE);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    
    return tex;
}
