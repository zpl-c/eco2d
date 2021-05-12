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

int32_t assets_setup(void) {
    for (uint32_t i=0; i<ASSETS_COUNT; i++) {
        asset *b = &assets[i];
        
        switch (b->kind) {
            case AKIND_TEXTURE: {
                b->tex = texgen_build_sprite(b->id);
            }break;
            
            case AKIND_SOUND: {
                // TODO(zaklaus): soundgen
            }break;
            
            default: {
                // TODO(zaklaus): assert
            }break;
        }
    }
    return 0;
}

void assets_destroy(void) {
    for (uint32_t i=0; i<ASSETS_COUNT; i++) {
        switch (assets[i].kind) {
            case AKIND_TEXTURE: {
                UnloadTexture(assets[i].tex);
            }break;
            
            case AKIND_SOUND: {
                // TODO(zaklaus): soundgen
            }break;
            
            default: {
                // TODO(zaklaus): assert
            }break;
        }
        
    }
}

uint16_t assets_find(asset_id id) {
    for (uint32_t i=0; i<ASSETS_COUNT; i++) {
        if (assets[i].id == id)
            return i;
    }
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
