#include "zpl.h"
#include "prediction.h"
#include "world/world.h"

#define PREDICT_SMOOTH_FACTOR_LO 0.8
#define PREDICT_SMOOTH_FACTOR_HI 0.12

float smooth_val(float cur, float tgt, float dt) {
#if 0
    return zpl_lerp(cur, tgt, zpl_lerp(PREDICT_SMOOTH_FACTOR_HI, PREDICT_SMOOTH_FACTOR_LO, zpl_unlerp(dt, WORLD_TRACKER_UPDATE_FAST_MS, WORLD_TRACKER_UPDATE_SLOW_MS)));
#endif
    return zpl_lerp(cur, tgt, PREDICT_SMOOTH_FACTOR_HI);
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
