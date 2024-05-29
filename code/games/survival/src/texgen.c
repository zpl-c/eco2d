#include "gen/texgen.h"
#include "world/world.h"
#include "zpl.h"
#include "utils/raylib_helpers.h"
#include "game.h"

Texture2D texgen_build_anim(asset_id id, int64_t counter) {
    (void)counter;
    switch (id) {

		// Mobs
		case ASSET_MOB: return LoadTexEco("enemy1");
        default: return texgen_build_anim_fallback(id, counter); break;
    }
}

Texture2D texgen_build_sprite(asset_id id) {
    switch (id) {
		case ASSET_PLAYER: return LoadTexEco("player");
        default: return texgen_build_sprite_fallback(id); break;
    }
}
