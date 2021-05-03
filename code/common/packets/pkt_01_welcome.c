// #include "packet.h"

// PACKET_GENERATE_ENCODE(1, 2, )
//
//#include "packet.h"
//#include "cwpack/cwpack.h"
//
//#define PKT_01_WELCOME_ID 1
//#define PKT_01_WELCOME_ARGS 2
//
//
//size_t pkt_01_welcome_encode(pkt_01_welcome *table) {
    //cw_pack_context pc = {0};
    //cw_pack_context_init(&pc, buffer, 20, 0);
    //cw_pack_array_size(&pc, 1 + PKT_01_WELCOME_ARGS);
    //cw_pack_signed(&pc, PKT_01_WELCOME_ID);
//
    //cw_pack_unsigned(&pc, chunk_size);
    //cw_pack_unsigned(&pc, chunk_amount);
//
    //return pc.current - pc.start; /* length */
//}
//
//int32_t pkt_01_welcome_decode(pkt_01_welcome *table, pkt_header *header) {
    //cw_unpack_context uc = {0};
    //cw_unpack_context_init(&uc, header->data, header->datalen, 0);
//
    //cw_unpack_next(&uc);
    //if (uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != PKT_01_WELCOME_ARGS) {
        //return -1;//todo: error
    //}
//
    //cw_unpack_next(&uc);
    //if (uc.item.type != CWP_ITEM_POSITIVE_INTEGER) return -1; // expected chunk size
    //table->chunk_size = uc.item.as.u64;
//
    //cw_unpack_next(&uc);
    //if (uc.item.type != CWP_ITEM_POSITIVE_INTEGER) return -1; // expected chunk amount
    //table->chunk_amount = uc.item.as.u64;
//
    //if (uc.return_code != CWP_RC_OK) return -1; // unpacking failed somwwhere
    //cw_unpack_next(&uc);
    //if (uc.return_code != CWP_RC_END_OF_INPUT) return -1; // not finished yet but should be?
//
    //return 0;
//}
//
//int32_t pkt_01_handler(pkt_header *header) {
    //pkt_01_welcome table;
    //pkt_01_welcome_decode(&table, header);
//
    //zpl_printf("we received: chunk_size: %d and chunk_amount: %d\n", table.chunk_size, table.chunk_amount);
//
    // do STUFF
    //return 0;
//}
//