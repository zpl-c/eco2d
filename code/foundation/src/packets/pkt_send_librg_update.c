#include "zpl.h"
#include "pkt/packet_utils.h"
#include "packets/pkt_send_librg_update.h"
#include "world/world.h"
#include "core/game.h"

size_t pkt_send_librg_update(uint64_t peer_id,
                             uint16_t view_id,
                             uint8_t ticker,
                             void *data,
                             size_t datalen) {
    return pkt_world_write(MSG_ID_LIBRG_UPDATE, pkt_send_librg_update_encode(data, (int32_t)datalen, ticker), 1, view_id, (void*)peer_id, 0);
}

size_t pkt_send_librg_update_encode(void *data, int32_t data_length, uint8_t layer_id) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, 2);
    cw_pack_unsigned(&pc, layer_id);
    cw_pack_bin(&pc, data, data_length);
    return pkt_pack_msg_size(&pc);
}

#define NUM_SAMPLES 128

static float smooth_time(float time) {
    static float time_samples[NUM_SAMPLES] = {};
    static int32_t curr_index = 0;

    time_samples[curr_index] = time;
    if (++curr_index == NUM_SAMPLES)
        curr_index = 0;

    float average = 0;
    for (int32_t i = NUM_SAMPLES; i--; )
        average += time_samples[i];
    average /= NUM_SAMPLES;

    time = zpl_min(time, average * 2);
    return time;
}

#undef NUM_SAMPLES

int32_t pkt_send_librg_update_handler(pkt_header *header) {
    cw_unpack_context uc = {0};
    pkt_unpack_msg(&uc, header, 2);
    cw_unpack_next(&uc);
    
    if (uc.item.type != CWP_ITEM_POSITIVE_INTEGER)
        return -1;
    
    uint8_t layer_id = (uint8_t)uc.item.as.u64;
    
    cw_unpack_next(&uc);
    
    if (uc.item.type != CWP_ITEM_BIN)
        return -1;
    
    world_view *view = game_world_view_get(header->view_id);
    view->active_layer_id = layer_id;
    
    int32_t state = librg_world_read(view->tracker, header->view_id, uc.item.as.bin.start, uc.item.as.bin.length, NULL);
    if (state < 0) zpl_printf("[ERROR] world read error: %d\n", state);
    
    float now = (float)get_cached_time();
    view->delta_time[layer_id] = smooth_time(now - view->last_update[layer_id]);
    view->last_update[layer_id] = now;
    
    return state;
}
