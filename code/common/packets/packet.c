#include "packet.h"

#define PKT_HEADER_ELEMENTS 2

pkt_handler pkt_handlers[] = {
    {.id = MSG_ID_01_WELCOME, .handler = pkt_01_welcome_handler},
};

int32_t pkt_header_encode(pkt_header *table) {
    return 0;
}

int32_t pkt_header_decode(pkt_header *table, void *data, size_t datalen) {
    cw_unpack_context uc = {0};
    cw_unpack_context_init(&uc, data, datalen, 0);

    cw_unpack_next(&uc);
    if (uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != PKT_HEADER_ELEMENTS) {
        return -1;//todo: error
    }

    cw_unpack_next(&uc);
    if (uc.item.type != CWP_ITEM_POSITIVE_INTEGER || uc.item.as.u64 > UINT16_MAX) {
        return -1; // invalid packet id id
    }

    uint16_t pkt_id = (uint16_t)uc.item.as.u64;

    cw_unpack_next(&uc);
    const void *packed_blob = uc.item.as.bin.start;
    uin32_t packed_size = uc.item.as.bind.length;

    table->id = pkt_id;
    table->data = packed_blob;
    table->datalen = packed_size;
    table->ok = 1;

    if (uc.return_code != CWP_RC_OK) return -1; // unpacking failed somwwhere
    cw_unpack_next(&uc);
    if (uc.return_code != CWP_RC_END_OF_INPUT) return -1; // not finished yet but should be?

    return 0;
}
