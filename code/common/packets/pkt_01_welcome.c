#include "pkt_01_welcome.h"
#include "world.h"

pkt_desc pkt_01_welcome_desc[] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, block_size) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, chunk_size) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, world_size) },
    { PKT_END },
};

size_t pkt_01_welcome_encode(pkt_01_welcome *table) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, 3);
    pkt_pack_struct(&pc, pkt_01_welcome_desc, PKT_STRUCT_PTR(table));
    return pkt_pack_msg_size(&pc);
}

int32_t pkt_01_welcome_handler(pkt_header *header) {
    pkt_01_welcome table;
    PKT_IF(pkt_msg_decode(header, pkt_01_welcome_desc, 3, PKT_STRUCT_PTR(&table)));

    zpl_printf("we received: block_size: %d, chunk_size: %d and world_size: %d\n", table.block_size, table.chunk_size, table.world_size);
    
    world_init_minimal(table.block_size, table.chunk_size, table.world_size, NULL, NULL);
    return 0;
}
