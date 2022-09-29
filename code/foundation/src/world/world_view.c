#include "zpl.h"
#include "world/world_view.h"
#include "world/entity_view.h"
#include "world/prediction.h"
#include "librg.h"
#include "world/world.h"
#include "core/game.h"

#include <math.h>

int32_t tracker_read_remove(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
    world_view *view = (world_view*)librg_world_userdata_get(w);
    entity_view_remove_chunk_texture(&view->entities, entity_id);
    entity_view_destroy(&view->entities, entity_id);
    return 0;
}

int32_t tracker_read_update(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    world_view *view = (world_view*)librg_world_userdata_get(w);

    entity_view data = entity_view_unpack_struct(buffer, actual_length);
    entity_view *d = entity_view_get(&view->entities, entity_id);
#if 1
    if (d && d->layer_id < view->active_layer_id) {
        if ((get_cached_time()*1000.0f) - d->last_update > WORLD_TRACKER_UPDATE_NORMAL_MS) {
            d->layer_id = zpl_min(WORLD_TRACKER_LAYERS-1, d->layer_id+1);
        }
        // NOTE(zaklaus): reject updates from slower layers
        else return 0;
    }
#endif

    data.last_update = (uint64_t)(get_cached_time()*1000.0f);
    data.layer_id = view->active_layer_id;
    predict_receive_update(d, &data);
    entity_view_update_or_create(&view->entities, entity_id, data);
    entity_view_remove_chunk_texture(&view->entities, entity_id);
    entity_view_update_chunk_texture(&view->entities, entity_id, view);
    return 0;
}

int32_t tracker_read_create(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    world_view *view = (world_view*)librg_world_userdata_get(w);

    entity_view data = entity_view_unpack_struct(buffer, actual_length);
    data.ent_id = entity_id;
    data.layer_id = view->active_layer_id;
    data.tran_time = 0.0f;
    data.color = rand(); // TODO(zaklaus): feed from server
    if (data.flag & EFLAG_INTERP) {
        data.tx = data.x;
        data.ty = data.y;
        data.theading = data.heading;
    }
    entity_view_update_or_create(&view->entities, entity_id, data);
    entity_view_mark_for_fadein(&view->entities, entity_id);
    entity_view_update_chunk_texture(&view->entities, entity_id, view);
    return 0;
}

world_view world_view_create(uint16_t view_id) {
    world_view view = {0};
    view.view_id = view_id;
    view.tracker = librg_world_create();
    entity_view_init(&view.entities);
    return view;
}

void world_view_init(world_view *view, uint32_t seed, uint64_t ent_id, uint16_t chunk_size, uint16_t chunk_amount) {
    view->seed = seed;
    view->owner_id = ent_id;
    view->chunk_size = chunk_size;
    view->chunk_amount = chunk_amount;
    view->dim = WORLD_BLOCK_SIZE * chunk_size * chunk_amount;
    view->size = view->dim * view->dim;

    librg_config_chunksize_set(view->tracker, WORLD_BLOCK_SIZE * chunk_size, WORLD_BLOCK_SIZE * chunk_size, 1);
    librg_config_chunkamount_set(view->tracker, chunk_amount, chunk_amount, 1);
    librg_config_chunkoffset_set(view->tracker, LIBRG_OFFSET_BEG, LIBRG_OFFSET_BEG, LIBRG_OFFSET_BEG);

    librg_event_set(view->tracker, LIBRG_READ_CREATE, tracker_read_create);
    librg_event_set(view->tracker, LIBRG_READ_REMOVE, tracker_read_remove);
    librg_event_set(view->tracker, LIBRG_READ_UPDATE, tracker_read_update);
    librg_world_userdata_set(view->tracker, view);
}

void world_view_destroy(world_view *view) {
    librg_world_destroy(view->tracker);
    entity_view_free(&view->entities);
}
