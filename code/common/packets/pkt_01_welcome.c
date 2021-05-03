#include "packet.h"
#include "cwpack/cwpack.h"
#include "packet_utils.h"

#define PKT_01_WELCOME_ARGS 2

pkt_desc pkt_01_welcome_desc[3] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, chunk_size) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, chunk_amount) },
    { PKT_END },
};

size_t pkt_01_welcome_encode(pkt_01_welcome *table) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, PKT_01_WELCOME_ARGS);

    cw_pack_unsigned(&pc, world_chunk_size());
    cw_pack_unsigned(&pc, world_chunk_amount());

    return pc.current - pc.start; /* length */
}

int32_t pkt_01_welcome_decode(pkt_01_welcome *table, pkt_header *header) {
    cw_unpack_context uc = {0};
    pkt_unpack_msg(&uc, header, PKT_01_WELCOME_ARGS);
    pkt_unpack_struct(&uc, header, PKT_STRUCT_PTR(table));

    return pkt_validate_eof_msg(&uc);
}

int32_t pkt_01_welcome_handler(pkt_header *header) {
#if 1
    pkt_01_welcome table;
    pkt_01_welcome_decode(&table, header);

    zpl_printf("we received: chunk_size: %d and chunk_amount: %d\n", table.chunk_size, table.chunk_amount);
#endif
    return 0;
}
