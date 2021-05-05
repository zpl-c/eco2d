#include "packet_utils.h"
#include "packets/pkt_send_librg_update.h"
#include "world/world.h"

size_t pkt_send_librg_update_encode(void *data, int32_t data_length) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, 1);
    cw_pack_bin(&pc, data, data_length);
    return pkt_pack_msg_size(&pc);
}

int32_t pkt_send_librg_update_handler(pkt_header *header) {
    cw_unpack_context uc = {0};
    pkt_unpack_msg(&uc, header, 1);
    cw_unpack_next(&uc);

    if (uc.item.type != CWP_ITEM_BIN)
        return -1;

    return librg_world_read(world_tracker(), 1, uc.item.as.bin.start, uc.item.as.bin.length, NULL);
}
