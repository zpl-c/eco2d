#pragma once
#include "zpl.h"
#include "packet.h"
#include "cwpack/cwpack.h"

inline void pkt_pack_msg(cw_pack_context *pc, uint32_t args) {
    cw_pack_context_init(pc, pkt_buffer, PKT_BUFSIZ, 0);
    cw_pack_array_size(pc, args);
}

inline int32_t pkt_unpack_msg(cw_unpack_context *uc, pkt_header *header, uint32_t args) {
    cw_unpack_context_init(uc, header->data, header->datalen, 0);
    
    cw_unpack_next(uc);
    if (uc->item.type != CWP_ITEM_ARRAY || uc->item.as.array.size != args) {
        return -1;//todo: error
    }
    
    return 0;
}

inline int32_t pkt_unpack_msg_raw(cw_unpack_context *uc, uint8_t *data, uint32_t datalen, uint32_t args) {
    pkt_header header = {.data = data, .datalen = datalen};
    return pkt_unpack_msg(uc, &header, args);
}

inline int32_t pkt_validate_eof_msg(cw_unpack_context *uc) {
    if (uc->return_code != CWP_RC_OK) return -1; // unpacking failed somwwhere
    cw_unpack_next(uc);
    if (uc->return_code != CWP_RC_END_OF_INPUT) return -1; // not finished yet but should be?
    return 0;
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
#define PKT_STRUCT_PTR(a) (void*)a, (uint32_t)sizeof(*a)
#endif

#ifndef PKT_FIELD
#define PKT_FIELD(k, t, a) .type = k, .offset = PKT_OFFSETOF(t, a), .size = PKT_FIELD_SIZEOF(t,a)
#endif

#ifndef PKT_END
#define PKT_END .type = CWP_NOT_AN_ITEM
#endif

typedef struct pkt_desc {
    cwpack_item_types type;
    size_t offset;
    size_t size;
} pkt_desc;
    
int32_t pkt_unpack_struct(cw_unpack_context *uc, pkt_desc *desc, void *raw_blob, uint32_t blob_size);
