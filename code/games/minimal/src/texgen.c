#include "gen/texgen.h"
#include "world/world.h"
#include "zpl.h"
#include "utils/raylib_helpers.h"

Texture2D texgen_build_anim(asset_id id, int64_t counter) {
    (void)counter;
    switch (id) {
        default: return GenColorEco(ColorFromHSV(id * 240.0f, 0.6f, 0.45f)); break;
    }
}

Texture2D texgen_build_sprite(asset_id id) {
    switch (id) {
        case ASSET_BLANK: return GenColorEco(WHITE); break;
        case ASSET_BUILDMODE_HIGHLIGHT: return GenColorEco(WHITE); break;

        default: return GenColorEco(ColorFromHSV(id * 240.0f, 0.6f, 0.45f)); break;
    }
}
