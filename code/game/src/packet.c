#include "packet_utils.h"
#include "compress.h"
#include "cwpack/cwpack.h"

// NOTE(zaklaus): packets

#include "packets/pkt_00_init.h"
#include "packets/pkt_01_welcome.h"
#include "packets/pkt_send_keystate.h"
#include "packets/pkt_send_librg_update.h"
#include "packets/pkt_switch_viewer.h"

#define PKT_HEADER_ELEMENTS 3

pkt_handler pkt_handlers[] = {
    {.id = MSG_ID_00_INIT, .handler = pkt_00_init_handler},
    {.id = MSG_ID_01_WELCOME, .handler = pkt_01_welcome_handler},
    {.id = MSG_ID_LIBRG_UPDATE, .handler = pkt_send_librg_update_handler},
    {.id = MSG_ID_SEND_KEYSTATE, .handler = pkt_send_keystate_handler},
    {.id = MSG_ID_SWITCH_VIEWER, .handler = pkt_switch_viewer_handler},
};

uint8_t pkt_buffer[PKT_BUFSIZ];

int32_t pkt_header_encode(pkt_messages id, uint16_t view_id, void *data, size_t datalen) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, PKT_HEADER_ELEMENTS);
    cw_pack_unsigned(&pc, id);
    cw_pack_unsigned(&pc, view_id);
    cw_pack_bin(&pc, data, (uint32_t)datalen);
    return (int32_t)pkt_pack_msg_size(&pc);
}

int32_t pkt_header_decode(pkt_header *table, void *data, size_t datalen) {
    cw_unpack_context uc = {0};
    pkt_unpack_msg_raw(&uc, data, (uint32_t)datalen, PKT_HEADER_ELEMENTS);
    
    cw_unpack_next(&uc);
    if (uc.item.type != CWP_ITEM_POSITIVE_INTEGER || uc.item.as.u64 > UINT16_MAX) {
        return -1; // invalid packet id
    }
    
    uint16_t pkt_id = (uint16_t)uc.item.as.u64;
    
    cw_unpack_next(&uc);
    if (uc.item.type != CWP_ITEM_POSITIVE_INTEGER || uc.item.as.u64 > UINT16_MAX) {
        return -1; // invalid view id
    }
    
    uint16_t view_id = (uint16_t)uc.item.as.u64;
    
    cw_unpack_next(&uc);
    const void *packed_blob = uc.item.as.bin.start;
    uint32_t packed_size = uc.item.as.bin.length;
    
    table->id = pkt_id;
    table->view_id = view_id;
    table->data = (void *)packed_blob;
    table->datalen = packed_size;
    table->ok = 1;
    
    return pkt_validate_eof_msg(&uc) != -1;
}

int32_t pkt_unpack_struct(cw_unpack_context *uc, pkt_desc *desc, void *raw_blob, uint32_t blob_size) {
    uint8_t *blob = (uint8_t*)raw_blob;
    for (pkt_desc *field = desc; field->type != CWP_NOT_AN_ITEM; ++field) {
        cw_unpack_next(uc);
        if (field->skip_count != 0) {
            if (uc->item.type != CWP_ITEM_POSITIVE_INTEGER) return -1; // unexpected field
            field += uc->item.as.u64;
            continue;
        }
        if (uc->item.type != field->type) return -1; // unexpected field
        if (blob + field->offset + field->size > blob + blob_size) return -1; // field does not fit
        switch (field->type) {
            case CWP_ITEM_DOUBLE: {
                zpl_memcopy(blob + field->offset, (uint8_t*)&uc->item.as.long_real, field->size);
            }break;
            case CWP_ITEM_FLOAT: {
                zpl_memcopy(blob + field->offset, (uint8_t*)&uc->item.as.real, field->size);
            } break;
            case CWP_ITEM_NEGATIVE_INTEGER: {
                zpl_memcopy(blob + field->offset, (uint8_t*)&uc->item.as.i64, field->size);
            }break;
            case CWP_ITEM_POSITIVE_INTEGER: {
                zpl_memcopy(blob + field->offset, (uint8_t*)&uc->item.as.u64, field->size);
            }break;
            case CWP_ITEM_BIN: {
                if (uc->item.as.bin.length >= PKT_BUFSIZ) return -1; // bin blob too big
                static uint8_t bin_buf[PKT_BUFSIZ] = {0};
                uint32_t actual_size = decompress_rle((void *)uc->item.as.bin.start, uc->item.as.bin.length, bin_buf);
                if (actual_size != field->size) return -1; // bin size mismatch
                zpl_memcopy(blob + field->offset, bin_buf, actual_size);
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
    for (pkt_desc *field = desc; field->type != CWP_NOT_AN_ITEM; ++field) {
        if (field->skip_count != 0) {
            uint8_t val = *(uint8_t*)(blob + field->offset);
            if ((field->skip_count > 0 && val == field->skip_eq) ||
                (field->skip_count < 0 && val != field->skip_eq)) {
                cw_pack_unsigned(pc, zpl_abs(field->skip_count));
                field += zpl_abs(field->skip_count);
            } else {
                cw_pack_unsigned(pc, 0);
            }
            
            continue;
        }
        
        switch (field->type) {
            case CWP_ITEM_BIN: {
                if (field->size >= PKT_BUFSIZ) return -1; // bin blob too big
                static uint8_t bin_buf[PKT_BUFSIZ] = {0};
                uint32_t size = compress_rle((void *)(blob + field->offset), (uint32_t)field->size, bin_buf);
                cw_pack_bin(pc, bin_buf, size);
            }break;
            case CWP_ITEM_POSITIVE_INTEGER: {
                uint64_t num;
                zpl_memcopy(&num, blob + field->offset, field->size);
                cw_pack_unsigned(pc, num);
            }break;
            case CWP_ITEM_NEGATIVE_INTEGER: {
                int64_t num;
                zpl_memcopy(&num, blob + field->offset, field->size);
                cw_pack_signed(pc, num);
            }break;
            case CWP_ITEM_DOUBLE: {
                double num;
                zpl_memcopy(&num, blob + field->offset, field->size);
                cw_pack_double(pc, num);
            }break;
            case CWP_ITEM_FLOAT: {
                float num;
                zpl_memcopy(&num, blob + field->offset, field->size);
                cw_pack_float(pc, num);
            }break;
            default: {
                zpl_printf("[WARN] unsupported pkt field type %lld !\n", field->type); 
                return -1; // unsupported field
            }break;
        }
    }
    
    return 0;
}

void pkt_dump_struct(pkt_desc *desc, void* raw_blob, uint32_t blob_size) {
    uint8_t *blob = (uint8_t*)raw_blob;
    zpl_printf("{\n");
    for (pkt_desc *field = desc; field->type != CWP_NOT_AN_ITEM; ++field) {
        if (field->skip_count) {
            uint8_t val = *(uint8_t*)(blob + field->offset);
            if (val == field->skip_eq) {
                field += field->skip_count;
            }
            
            continue;
        }
        zpl_printf("  \"%s\": ", field->name);
        switch (field->type) {
            case CWP_ITEM_BIN: {
                // TODO(zaklaus): print memory dump as array of hex bytes
            }break;
            case CWP_ITEM_POSITIVE_INTEGER: {
                zpl_printf("%u\n", *(uint64_t*)(blob + field->offset));
            }break;
            case CWP_ITEM_NEGATIVE_INTEGER: {
                zpl_printf("%d\n", *(int64_t*)(blob + field->offset));
            }break;
            case CWP_ITEM_FLOAT: {
                zpl_printf("%f\n", *(float*)(blob + field->offset));
            }break;
            case CWP_ITEM_DOUBLE: {
                zpl_printf("%f\n", *(double*)(blob + field->offset));
            }break;
            default: {
                zpl_printf("[WARN] unsupported pkt field type %lld !\n", field->type); 
                return; // unsupported field
            }break;
        }
    }
    zpl_printf("}\n");
}
