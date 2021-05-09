#include "zpl.h"
#include "prediction.h"
#include "world/world.h"
#include "game.h"

#define PREDICT_SMOOTH_FACTOR_LO 0.80
#define PREDICT_SMOOTH_FACTOR_HI 0.12

static inline float map_factor(float x) {
    x = 1.0f - zpl_clamp01(x);
    return 1.0f - x*x*x*x*x*x*x*x;
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

void predict_receive_update(entity_view *d, entity_view *data) {
    if (d && data->flag & EFLAG_INTERP) {
        // NOTE(zaklaus): store target pos but keep x,y unchanged
        float tx = data->x;
        float ty = data->y;
        data->x = d->x;
        data->y = d->y;
        data->tx = tx;
        data->ty = ty;
    }
}
