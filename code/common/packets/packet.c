#include "packet.h"
#include "packet_utils.h"
#include "cwpack/cwpack.h"

#define PKT_HEADER_ELEMENTS 2

pkt_handler pkt_handlers[] = {
    #ifdef SERVER
    #else
    {.id = MSG_ID_01_WELCOME, .handler = pkt_01_welcome_handler},
    #endif
};

uint8_t pkt_buffer[PKT_BUFSIZ];
uint8_t pkt_pack_buffer[PKT_BUFSIZ];

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

int32_t pkt_unpack_struct(cw_unpack_context *uc, pkt_desc *desc, void *raw_blob, uint32_t blob_size) {
    uint8_t *blob = (uint8_t*)raw_blob;
    for (pkt_desc *field = desc; field->type != CWP_NOT_AN_ITEM; ++field) {
        cw_unpack_next(uc);
        if (uc->item.type != field->type) return -1; // unexpected field
        if (blob + field->offset + field->size > blob + blob_size) return -1; // field does not fit
        switch (field->type) {
            case CWP_ITEM_POSITIVE_INTEGER: {
                zpl_memcopy(blob + field->offset, (uint8_t*)&uc->item.as.u64, field->size);
            }break;
            case CWP_ITEM_BIN: {
                if (uc->item.as.bin.length != field->size) return -1; // bin size mismatch
                zpl_memcopy(blob + field->offset, (uint8_t*)uc->item.as.bin.start, uc->item.as.bin.length);
            }break;
            default: {
                zpl_printf("[WARN] unsupported pkt field type %lld !\n", field->type); 
                return -1; // unsupported field
            }break;
        }
    }
    
    return 0;
}

int32_t pkt_pack_struct(cw_pack_context *pc, pkt_desc *desc, void *raw_blob, uint32_t blob_size) {
    uint8_t *blob = (uint8_t*)raw_blob;
    zpl_zero_item(pkt_pack_buffer);
    for (pkt_desc *field = desc; field->type != CWP_NOT_AN_ITEM; ++field) {
        switch (field->type) {
            case CWP_ITEM_BIN: {
                cw_pack_bin(pc, blob + field->offset, field->size);
            }break;
            case CWP_ITEM_POSITIVE_INTEGER: {
                uint64_t num;
                zpl_memcopy(&num, blob + field->offset, field->size);
                cw_pack_unsigned(pc, num);
            }break;
            default: {
                zpl_printf("[WARN] unsupported pkt field type %lld !\n", field->type); 
                return -1; // unsupported field
            }break;
        }
    }
    
    return 0;
}
