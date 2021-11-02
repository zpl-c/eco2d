#include "gen/texgen.h"
#include "world/world.h"
#include "texgen_data.c"

#define ZPL_NANO
#include "zpl.h"

static inline
Texture2D LoadImageEco(const char *name) {
    static char filename[128];
    zpl_snprintf(filename,  128, "art/gen/%s.png", name);
    return LoadTexture(filename);
}

Texture2D texgen_build_sprite(asset_id id) {
    switch (id) {
        case ASSET_DEMO_ICEMAKER: return LoadImageEco("demo_icemaker");
        
        // NOTE(zaklaus): blocks
        case ASSET_FENCE: return LoadImageEco("fence");
        case ASSET_GROUND: return LoadImageEco("grass");
        case ASSET_DIRT: return LoadImageEco("dirt");
        case ASSET_WALL: return LoadImageEco("asphalt");
        case ASSET_HILL_SNOW:
        case ASSET_HILL: return LoadImageEco("rock");
        case ASSET_WATER: return LoadImageEco("water");
        case ASSET_LAVA: return LoadImageEco("lava");
        case ASSET_WOOD: return LoadImageEco("wood");
        case ASSET_TREE: return LoadImageEco("tree");
        case ASSET_BELT_LEFT: return LoadImageEco("belt_left");
        case ASSET_BELT_RIGHT: return LoadImageEco("belt_right");
        case ASSET_BELT_UP: return LoadImageEco("belt_up");
        case ASSET_BELT_DOWN: return LoadImageEco("belt_down");
        
        default: {
            Image img = GenImageColor(1, 1, RAYWHITE);
            Texture2D tex = LoadTextureFromImage(img);
            UnloadImage(img);
            return tex;
        }break;
    }
    
    ZPL_PANIC("unreachable code");
}
