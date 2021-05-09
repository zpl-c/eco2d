#include "zpl.h"
#include "packet_utils.h"
#include "packets/pkt_send_librg_update.h"
#include "world/world.h"
#include "game.h"

size_t pkt_send_librg_update_encode(void *data, int32_t data_length, uint8_t layer_id) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, 2);
    cw_pack_unsigned(&pc, layer_id);
    cw_pack_bin(&pc, data, data_length);
    return pkt_pack_msg_size(&pc);
}

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
    
    uint64_t now = zpl_time_rel_ms();
    view->delta_time[layer_id] = now - view->last_update[layer_id];
    view->last_update[layer_id] = now;
    
    return state;
}
