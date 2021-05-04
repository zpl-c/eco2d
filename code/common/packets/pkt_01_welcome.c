#include "packet.h"
#include "packet_utils.h"

#ifdef SERVER
size_t pkt_01_welcome_encode(pkt_01_welcome *table) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, PKT_01_WELCOME_ARGS);

    cw_pack_unsigned(&pc, world_chunk_size());
    cw_pack_unsigned(&pc, world_chunk_amount());

    return pkt_pack_msg_size(&pc);
}
#else
pkt_desc pkt_01_welcome_desc[3] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, chunk_size) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, chunk_amount) },
    { PKT_END },
};

int32_t pkt_01_welcome_handler(pkt_header *header) {
    pkt_01_welcome table;
    PKT_IF(pkt_msg_decode(header, pkt_01_welcome_desc, 2, PKT_STRUCT_PTR(&table)));

    zpl_printf("we received: chunk_size: %d and chunk_amount: %d\n", table.chunk_size, table.chunk_amount);
    return 0;
}
#endif