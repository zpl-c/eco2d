#include "gen/assets.h"
#include "raylib.h"
#include "gen/texgen.h"

typedef struct {
    asset_id id;
    asset_kind kind;

    union {
        Texture2D tex;
        Sound snd;
    };

    // NOTE(zaklaus): metadata
} asset;

#include "assets_list.c"

#define ASSET_FRAME_RENDER_MS (1.0/5.0)
#define ASSET_FRAME_SKIP 4
static int64_t assets_resources_frame_counter = 1;
static double assets_resources_frame_next_draw = 0.0;

#include <time.h>

void assets_register(asset_desc a) {
    if (!assets) {
        zpl_array_init(assets, zpl_heap());
    }

    zpl_array_append(assets, ((asset){ .id = a.id, .kind = a.kind }));
}

void assets_cleanup(void) {
    zpl_array_free(assets); assets = NULL;
}

int32_t assets_resources_setup(void) {
    for (zpl_isize i=0; i<zpl_array_count(assets); i++) {
        asset *b = &assets[i];

        switch (b->kind) {
            case AKIND_TEXTURE: {
                b->tex = texgen_build_sprite(b->id);
            }break;

            case AKIND_ANIM: {
                b->tex = texgen_build_anim(b->id, 0);
            }break;

            case AKIND_SOUND: {
                // TODO(zaklaus): soundgen
            }break;

            default: break;
        }
    }
    assets_resources_frame_next_draw = get_cached_time() + ASSET_FRAME_RENDER_MS;
    return 0;
}

int32_t assets_resources_frame(void) {
    if (assets_resources_frame_next_draw < get_cached_time()) {
        for (zpl_isize i=0; i<zpl_array_count(assets); i++) {
            asset *b = &assets[i];

            switch (b->kind) {
                case AKIND_ANIM: {
                    UnloadTexture(b->tex);
                    b->tex = texgen_build_anim(b->id, assets_resources_frame_counter);
                }break;

                default: break;
            }
        }

        assets_resources_frame_next_draw = get_cached_time() + ASSET_FRAME_RENDER_MS;
        assets_resources_frame_counter += ASSET_FRAME_SKIP;
    }

    return 0;
}

void assets_resources_destroy(void) {
    for (zpl_isize i=0; i<zpl_array_count(assets); i++) {
        switch (assets[i].kind) {
            case AKIND_ANIM:
            case AKIND_TEXTURE: {
                UnloadTexture(assets[i].tex);
            }break;

            case AKIND_SOUND: {
                // TODO(zaklaus): soundgen
            }break;

            default: break;
        }
    }
}

uint16_t assets_find(asset_id id) {
    for (zpl_isize i=0; i<zpl_array_count(assets); i++) {
        if (assets[i].id == id)
            return i;
    }

    ZPL_PANIC("Unknown asset id: %d\n", id);
    return ASSET_INVALID;
}

asset_kind assets_get_kind(uint16_t id) {
    return assets[id].kind;
}

void *assets_get_snd(uint16_t id) {
    return (void*)&assets[id].snd;;
}

void *assets_get_tex(uint16_t id) {
    return (void*)&assets[id].tex;
}
