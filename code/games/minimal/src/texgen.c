#include "gen/texgen.h"
#include "world/world.h"
#include "zpl.h"

static inline
Texture2D LoadTexEco(const char *name) {
    static char filename[128];
    zpl_snprintf(filename,  128, "art/gen/%s.png", name);
    return LoadTexture(filename);
}

static inline
Image LoadImageEco(const char *name) {
    static char filename[128];
    zpl_snprintf(filename,  128, "art/gen/%s.png", name);
    return LoadImage(filename);
}

static inline
Texture2D Image2TexEco(Image image) {
    Texture2D tex = LoadTextureFromImage(image);
    UnloadImage(image);
    return tex;
}

static inline
Texture2D GenColorEco(Color color) {
    Image img = GenImageColor(1, 1, color);
    return Image2TexEco(img);
}

Texture2D texgen_build_anim(asset_id id, int64_t counter) {
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

Texture2D texgen_build_sprite(asset_id id) {
    switch (id) {
        case ASSET_BLANK: return GenColorEco(WHITE); break;
        case ASSET_BUILDMODE_HIGHLIGHT: return GenColorEco(WHITE); break;

        // NOTE(zaklaus): items
        case ASSET_DEMO_ICEMAKER: return LoadTexEco("demo_icemaker");

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

        default: return GenColorEco(PINK); break;
    }
}
