#include "gen/texgen.h"
#include "world/world.h"
#include "zpl.h"
#include "utils/raylib_helpers.h"

Texture2D texgen_build_anim_fallback(asset_id id, int64_t counter) {
    (void)counter;
    switch (id) {
        case ASSET_WATER: {
            Image img = LoadImageEco("water");
            ImageColorBrightness(&img, zpl_abs((counter % 64 - 32)*2));
            return Image2TexEco(img);
        }break;

        default: return GenColorEco(PINK); break;
    }
}

Texture2D texgen_build_sprite_fallback(asset_id id) {
    switch (id) {
        case ASSET_BLANK: return GenColorEco(WHITE); break;
        case ASSET_BUILDMODE_HIGHLIGHT: return GenColorEco(WHITE); break;
        case ASSET_BLOCK_FRAME: return GenFrameRect(); break;
        case ASSET_BIG_TREE: return LoadTexEco("bigtree"); break;

        // NOTE(zaklaus): items
        case ASSET_COAL: return LoadTexEco("coal");
        case ASSET_BLUEPRINT: return LoadTexEco("blueprint");

        // NOTE(zaklaus): blocks
        case ASSET_FENCE: return LoadTexEco("fence");
        case ASSET_GROUND: return LoadTexEco("grass");
        case ASSET_DIRT: return LoadTexEco("dirt");
        case ASSET_WALL: return LoadTexEco("asphalt");
        case ASSET_HILL_SNOW:
        case ASSET_HILL: return LoadTexEco("rock");
        case ASSET_LAVA: return LoadTexEco("lava");
        case ASSET_WOOD: return LoadTexEco("wood");
        case ASSET_TREE: return LoadTexEco("tree");
        // case ASSET_WATER: return LoadTexEco("water");

        case ASSET_BELT:
        case ASSET_BELT_RIGHT: return LoadTexEco("belt_right");
        case ASSET_BELT_LEFT: return LoadTexEco("belt_left");
        case ASSET_BELT_UP: return LoadTexEco("belt_up");
        case ASSET_BELT_DOWN: return LoadTexEco("belt_down");

        // NOTE(zaklaus): devices
        case ASSET_CHEST: return LoadTexEco("chest");
        case ASSET_FURNACE: return LoadTexEco("furnace-export");

        default: return GenColorEco(PINK); break;
    }
}
