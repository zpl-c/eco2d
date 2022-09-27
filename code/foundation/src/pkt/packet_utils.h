#pragma once
#include "zpl.h"
#include "pkt/packet.h"
#include "cwpack/cwpack.h"

#ifndef PKT_IF
#define PKT_IF(c) if (c < 0) return -1;
#endif

static inline void pkt_pack_msg(cw_pack_context *pc, uint32_t args) {
    cw_pack_context_init(pc, pkt_buffer, PKT_BUFSIZ, 0);
    cw_pack_array_size(pc, args);
}

static inline int32_t pkt_unpack_msg(cw_unpack_context *uc, pkt_header *header, uint32_t args) {
    cw_unpack_context_init(uc, header->data, header->datalen, 0);
    
    cw_unpack_next(uc);
    if (uc->item.type != CWP_ITEM_ARRAY || uc->item.as.array.size != args) {
        return -1;//todo: error
    }
    
    return 0;
}

static inline int32_t pkt_unpack_msg_raw(cw_unpack_context *uc, uint8_t *data, uint32_t datalen, uint32_t args) {
    pkt_header header = {.data = data, .datalen = datalen};
    return pkt_unpack_msg(uc, &header, args);
}

static inline int32_t pkt_validate_eof_msg(cw_unpack_context *uc) {
    if (uc->return_code != CWP_RC_OK) return -1; // unpacking failed somwwhere
    cw_unpack_next(uc);
    if (uc->return_code != CWP_RC_END_OF_INPUT) return -1; // not finished yet but should be?
    return 0;
}

static inline size_t pkt_pack_msg_size(cw_pack_context *pc) {
    return pc->current - pc->start; // NOTE(zaklaus): length
}

static inline int32_t pkt_prep_msg(pkt_header *pkt, pkt_messages id, uint16_t view_id, size_t pkt_size, int8_t is_reliable, uint16_t channel_id) {
    zpl_zero_item(pkt);
    static uint8_t pkt_data[PKT_BUFSIZ] = {0};
    zpl_memcopy(pkt_data, pkt_buffer, pkt_size);
    
    pkt->data = pkt_buffer;
    pkt->view_id = view_id;
    pkt->datalen = pkt_header_encode(id, view_id, pkt_data, pkt_size);
    pkt->is_reliable = is_reliable;
    pkt->channel_id = channel_id;
    pkt->id = id;
    return 0;
}

extern int32_t world_write(pkt_header *pkt, void *udata);

static inline int32_t pkt_world_write(pkt_messages id, size_t pkt_size, int8_t is_reliable, uint16_t view_id, void *udata, uint16_t channel_id) {
    pkt_header pkt;
    PKT_IF(pkt_prep_msg(&pkt, id, view_id, pkt_size, is_reliable, channel_id));
    return world_write(&pkt, udata);
}

#ifndef PKT_OFFSETOF
#if defined(_MSC_VER) || defined(ZPL_COMPILER_TINYC)
#    define PKT_OFFSETOF(Type, element) ((size_t) & (((Type *)0)->element))
#else
#    define PKT_OFFSETOF(Type, element) __builtin_offsetof(Type, element)
#endif
#    define PKT_FIELD_SIZEOF(type, member) sizeof(((type *)0)->member)
#endif

#ifndef PKT_STRUCT_PTR
#define PKT_STRUCT_PTR(a) (void*)(a), (uint32_t)sizeof(*(a))
#endif

#ifndef PKT_FIELD
#define PKT_FIELD(k, t, a) .type = k, .offset = PKT_OFFSETOF(t, a), .size = PKT_FIELD_SIZEOF(t,a), .it_size = PKT_FIELD_SIZEOF(t,a), .name = #a
#endif

#ifndef PKT_UINT
#define PKT_UINT(t, a) .type = CWP_ITEM_POSITIVE_INTEGER, .offset = PKT_OFFSETOF(t, a), .size = PKT_FIELD_SIZEOF(t,a), .it_size = PKT_FIELD_SIZEOF(t,a), .name = #a
#endif

#ifndef PKT_SINT
#define PKT_SINT(t, a) .type = CWP_ITEM_NEGATIVE_INTEGER, .offset = PKT_OFFSETOF(t, a), .size = PKT_FIELD_SIZEOF(t,a), .it_size = PKT_FIELD_SIZEOF(t,a), .name = #a
#endif

#ifndef PKT_REAL
#define PKT_REAL(t, a) .type = CWP_ITEM_DOUBLE, .offset = PKT_OFFSETOF(t, a), .size = PKT_FIELD_SIZEOF(t,a), .it_size = PKT_FIELD_SIZEOF(t,a), .name = #a
#endif

#ifndef PKT_HALF
#define PKT_HALF(t, a) .type = CWP_ITEM_FLOAT, .offset = PKT_OFFSETOF(t, a), .size = PKT_FIELD_SIZEOF(t,a), .it_size = PKT_FIELD_SIZEOF(t,a), .name = #a
#endif

#ifndef PKT_ARRAY
#define PKT_ARRAY(t, a) .type = CWP_ITEM_BIN, .offset = PKT_OFFSETOF(t, a), .size = PKT_FIELD_SIZEOF(t,a), .it_size = PKT_FIELD_SIZEOF(t,a[0]), .name = #a
#endif

#ifndef PKT_SKIP_IF
#define PKT_SKIP_IF(t, a, e, n) .skip_count = n, .offset = PKT_OFFSETOF(t, a), .skip_eq = e, .name = #a
#endif

#ifndef PKT_KEEP_IF
#define PKT_KEEP_IF(t, a, e, n) .skip_count = -(n), .offset = PKT_OFFSETOF(t, a), .skip_eq = e, .name = #a
#endif

#ifndef PKT_END
#define PKT_END .type = CWP_NOT_AN_ITEM
#endif

typedef struct pkt_desc {
    const char *name;
    cwpack_item_types type;
    size_t offset;
    size_t size;
    size_t it_size;
    int16_t skip_count;
    uint8_t skip_eq;
} pkt_desc;

int32_t pkt_unpack_struct(cw_unpack_context *uc, pkt_desc *desc, void *raw_blob, uint32_t blob_size);
int32_t pkt_pack_struct(cw_pack_context *pc, pkt_desc *desc, void *raw_blob, uint32_t blob_size);

static inline int32_t pkt_msg_decode(pkt_header *header, pkt_desc* desc, uint32_t args, void *raw_blob, uint32_t blob_size) {
    cw_unpack_context uc = {0};
    PKT_IF(pkt_unpack_msg(&uc, header, args));
    PKT_IF(pkt_unpack_struct(&uc, desc, raw_blob, blob_size));
    
    return pkt_validate_eof_msg(&uc);
}

static inline uint32_t pkt_pack_desc_args(pkt_desc *desc) {
    uint32_t cnt = 0;
    for (pkt_desc *field = desc; field->type != CWP_NOT_AN_ITEM; ++field, ++cnt) {}
    return cnt;
}

static inline size_t pkt_table_encode(pkt_desc *desc, void *raw_blob, uint32_t blob_size) {
    assert(desc && raw_blob && blob_size > 0);
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, pkt_pack_desc_args(desc));
    pkt_pack_struct(&pc, desc, raw_blob, blob_size);
    return pkt_pack_msg_size(&pc);
}

void pkt_dump_struct(pkt_desc *desc, void* raw_blob, uint32_t blob_size);
