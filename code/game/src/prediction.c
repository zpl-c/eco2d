#include "zpl.h"
#include "prediction.h"
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
    float factor = zpl_clamp01(map_factor(zpl_unlerp(dt, WORLD_TRACKER_UPDATE_FAST_MS, WORLD_TRACKER_UPDATE_SLOW_MS)));
    
#if 0
    dt = 200;
    factor = map_factor(zpl_unlerp(dt, WORLD_TRACKER_UPDATE_FAST_MS, WORLD_TRACKER_UPDATE_SLOW_MS));
    zpl_printf("lerp factor: %f\n", factor);
    zpl_exit(0);
#endif
    
    return zpl_lerp(cur, tgt, zpl_lerp(PREDICT_SMOOTH_FACTOR_LO, PREDICT_SMOOTH_FACTOR_HI, factor));
}

float smooth_val_spherical(float cur, float tgt, uint64_t dt) {
    float factor = zpl_clamp01(map_factor(zpl_unlerp(dt, WORLD_TRACKER_UPDATE_FAST_MS, WORLD_TRACKER_UPDATE_SLOW_MS)));
    
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
