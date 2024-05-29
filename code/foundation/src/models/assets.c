#include "models/assets.h"
#include "lists/assets_ids.h"
#include "raylib.h"
#include "gen/texgen.h"
#include "models/database.h"

//#define ASSETS_COUNT (sizeof(assets)/sizeof(asset))

typedef struct {
    asset_id id;
    asset_kind kind;

    union {
        Texture2D tex;
        Sound snd;
    };

    // NOTE(zaklaus): metadata
} asset;

//#include "lists/assets_list.c"
static asset *assets;

#define ASSETS_COUNT (zpl_array_count(assets))

#define ASSET_FRAME_RENDER_MS (1.0/1.0)
#define ASSET_FRAME_SKIP 4
static int64_t assets_frame_counter = 1;
static double assets_frame_next_draw = 0.0;

#include <time.h>

void assets_db_init(void) {
    for (uint16_t i=0; i<MAX_ASSETS; i++) {
        db_exec(zpl_bprintf("INSERT INTO assets (id, name) VALUES (%d, '%s');", i, asset_names[i]+6));
    }
}

void assets_db(void) {
    zpl_array_init(assets, zpl_heap());
    db_push("SELECT * FROM resources;");
    for (size_t i=0, end=db_rows(); i<end; i++) {
        asset a={0};
        a.id = db_int("asset", i);
        a.kind = db_int("kind", i);
        zpl_array_append(assets, a);
    }
    db_pop();
}

int32_t assets_setup(void) {
    for (uint32_t i=0; i<ASSETS_COUNT; i++) {
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
    assets_frame_next_draw = get_cached_time() + ASSET_FRAME_RENDER_MS;
    return 0;
}

int32_t assets_frame(void) {
    if (assets_frame_next_draw < get_cached_time()) {
        for (uint32_t i=0; i<ASSETS_COUNT; i++) {
            asset *b = &assets[i];

            switch (b->kind) {
                case AKIND_ANIM: {
                    UnloadTexture(b->tex);
                    b->tex = texgen_build_anim(b->id, assets_frame_counter);
                }break;

                default: break;
            }
        }

        assets_frame_next_draw = get_cached_time() + ASSET_FRAME_RENDER_MS;
        assets_frame_counter += ASSET_FRAME_SKIP;
    }

    return 0;
}

void assets_destroy(void) {
    for (uint32_t i=0; i<ASSETS_COUNT; i++) {
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
    for (uint16_t i=0; i<ASSETS_COUNT; i++) {
        if (assets[i].id == id)
            return i;
    }

    //ZPL_PANIC("Unknown asset id: %d\n", id);
    return ASSET_INVALID;
}

asset_kind assets_get_kind(uint16_t id) {
    return assets[id].kind;
}

const char* assets_get_kind_name(uint16_t id) {
	static const char* names[] = { "Texture", "Animated Texture", "Sound" };
	return names[assets[id].kind];
}

void *assets_get_snd(uint16_t id) {
    return (void*)&assets[id].snd;;
}

void *assets_get_tex(uint16_t id) {
    return (void*)&assets[id].tex;
}

const char *asset_names[] = {
    #define X(id) #id,
    _ASSETS
    #undef X
};
