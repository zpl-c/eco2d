#include "assets.h"
#include "raylib.h"
#include "gen/texgen.h"

#define ASSETS_COUNT (sizeof(assets)/sizeof(asset))

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

#define ASSET_FRAME_RENDER_MS (1.0/10.0)
#define ASSET_FRAME_SKIP 2
static int64_t assets_frame_counter = 1;
static double assets_frame_next_draw = 0.0;

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
    
    assets_frame_next_draw = zpl_time_rel() + ASSET_FRAME_RENDER_MS;
    return 0;
}

int32_t assets_frame(void) {
    if (assets_frame_next_draw < zpl_time_rel()) {
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
        
        assets_frame_next_draw = zpl_time_rel() + ASSET_FRAME_RENDER_MS;
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
