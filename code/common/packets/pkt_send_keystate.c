#include "packet_utils.h"
#include "packets/pkt_send_keystate.h"

pkt_desc pkt_send_keystate_desc[] = {
    { PKT_FIELD(CWP_ITEM_DOUBLE, pkt_send_keystate, x) },
    { PKT_FIELD(CWP_ITEM_DOUBLE, pkt_send_keystate, y) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_send_keystate, use) },
    { PKT_END },
};

size_t pkt_send_keystate_encode(pkt_send_keystate *table) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, pkt_pack_desc_args(pkt_send_keystate_desc));
    pkt_pack_struct(&pc, pkt_send_keystate_desc, PKT_STRUCT_PTR(table));
    return pkt_pack_msg_size(&pc);
}

int32_t pkt_send_keystate_handler(pkt_header *header) {
    pkt_send_keystate table;
    PKT_IF(pkt_msg_decode(header, pkt_send_keystate_desc, pkt_pack_desc_args(pkt_send_keystate_desc), PKT_STRUCT_PTR(&table)));

    return 0;
}
