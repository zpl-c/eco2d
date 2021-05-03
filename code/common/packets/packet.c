#include "packet.h"
#include "packet_utils.h"
#include "cwpack/cwpack.h"

#define PKT_HEADER_ELEMENTS 2

pkt_handler pkt_handlers[] = {
    {.id = MSG_ID_01_WELCOME, .handler = pkt_01_welcome_handler},
};

uint8_t pkt_buffer[PKT_BUFSIZ];

int32_t pkt_header_encode(pkt_header *table) {
    return 0;
}

int32_t pkt_header_decode(pkt_header *table, void *data, size_t datalen) {
    cw_unpack_context uc = {0};
    pkt_unpack_msg_raw(&uc, data, datalen, PKT_HEADER_ELEMENTS);

    cw_unpack_next(&uc);
    if (uc.item.type != CWP_ITEM_POSITIVE_INTEGER || uc.item.as.u64 > UINT16_MAX) {
        return -1; // invalid packet id
    }

    uint16_t pkt_id = (uint16_t)uc.item.as.u64;

    cw_unpack_next(&uc);
    const void *packed_blob = uc.item.as.bin.start;
    uint32_t packed_size = uc.item.as.bin.length;

    table->id = pkt_id;
    table->data = packed_blob;
    table->datalen = packed_size;
    table->ok = 1;

    return pkt_validate_eof_msg(&uc);
}
