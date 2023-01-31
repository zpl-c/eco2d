#include "gen/texgen.h"
#include "world/world.h"
#include "zpl.h"
#include "utils/raylib_helpers.h"

Texture2D texgen_build_anim(asset_id id, int64_t counter) {
    (void)counter;
    switch (id) {
        default: return texgen_build_anim_fallback(id, counter); break;
    }
}

Texture2D texgen_build_sprite(asset_id id) {
    switch (id) {
        default: return texgen_build_sprite_fallback(id); break;
    }
}
