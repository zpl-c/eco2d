#include "zpl.h"
#include "prediction.h"
#include "platform.h"
#include "world/world.h"
#include "game.h"

#define PREDICT_SMOOTH_FACTOR_LO 0.10
#define PREDICT_SMOOTH_FACTOR_HI 0.01

static inline float map_factor(float x) {
    x = 1.0f - zpl_clamp01(x);
    return 1.0f - x*x*x*x*x*x*x*x;
}

static inline float base_angle(float x) {
    while (x > ZPL_TAU) x -= ZPL_TAU;
    while (x < 0.0f) x += ZPL_TAU;
    return x;
}

static inline float spherical_lerp(float a, float b, float t) {
    a = base_angle(a);
    b = base_angle(b);
    float d = b - a;
    
    if (d < -ZPL_PI) {
        b += ZPL_TAU;
    } else if (d > ZPL_PI) {
        b -= ZPL_TAU;
    }
    
    return base_angle(zpl_lerp(a, b, t));
}

float smooth_val(float cur, float tgt, uint64_t dt) {
    float factor = zpl_clamp01(map_factor(zpl_unlerp(dt, WORLD_TRACKER_UPDATE_MP_FAST_MS, WORLD_TRACKER_UPDATE_MP_SLOW_MS)));
    
#if 0
    dt = 200;
    factor = map_factor(zpl_unlerp(dt, WORLD_TRACKER_UPDATE_MP_FAST_MS, WORLD_TRACKER_UPDATE_MP_SLOW_MS));
    zpl_printf("lerp factor: %f\n", factor);
    zpl_exit(0);
#endif
    
    return zpl_lerp(cur, tgt, zpl_lerp(PREDICT_SMOOTH_FACTOR_LO, PREDICT_SMOOTH_FACTOR_HI, factor));
}

float smooth_val_spherical(float cur, float tgt, uint64_t dt) {
    float factor = zpl_clamp01(map_factor(zpl_unlerp(dt, WORLD_TRACKER_UPDATE_MP_FAST_MS, WORLD_TRACKER_UPDATE_MP_SLOW_MS)));
    
    return spherical_lerp(cur, tgt, zpl_lerp(PREDICT_SMOOTH_FACTOR_LO, PREDICT_SMOOTH_FACTOR_HI, factor));
}

void predict_receive_update(entity_view *d, entity_view *data) {
    if (d && data->flag & EFLAG_INTERP) {
        // NOTE(zaklaus): store target pos but keep x,y unchanged
        float tx = data->x;
        float ty = data->y;
        float theading = data->heading;
        data->x = d->x;
        data->y = d->y;
        data->heading = d->heading;
        data->tx = tx;
        data->ty = ty;
        data->theading = theading;
    }
    
    data->tran_effect = d->tran_effect;
    data->tran_time = d->tran_time;
}

#define ENTITY_DO_LERP_SP 1

void lerp_entity_positions(uint64_t key, entity_view *data) {
    (void)key;
    world_view *view = game_world_view_get_active();
    
    if (data->flag == EFLAG_INTERP) {
#if ENTITY_DO_LERP_SP==0
        if (game_get_kind() == GAMEKIND_CLIENT)
#else
        if (1)
#endif
        {
            data->x = smooth_val(data->x, data->tx, view->delta_time[data->layer_id]);
            data->y = smooth_val(data->y, data->ty, view->delta_time[data->layer_id]);
            data->heading = smooth_val_spherical(data->heading, data->theading, view->delta_time[data->layer_id]);
        } else {
            (void)view;
            data->x = data->tx;
            data->y = data->ty;
            data->heading = data->theading;
        }
    }
}

void do_entity_fadeinout(uint64_t key, entity_view * data) {
    (void)key;
    switch (data->tran_effect) {
        case ETRAN_FADEIN: {
            data->tran_time += platform_frametime();
            
            if (data->tran_time > 1.0f) {
                data->tran_effect = ETRAN_NONE;
                data->tran_time = 1.0f;
            }
        }break;
        
        case ETRAN_FADEOUT: {
            data->tran_time -= platform_frametime();
            
            if (data->tran_time < 0.0f) {
                data->tran_effect = ETRAN_REMOVE;
                data->tran_time = 0.0f;
            }
        }break;
        
        default: break;
    }
}
