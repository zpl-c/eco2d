#pragma once
#include "system.h"

#define PKT_BUFSIZ 16384

typedef enum {
    MSG_ID_00_INIT,
    MSG_ID_01_WELCOME,
    MSG_ID_LIBRG_UPDATE,
    MSG_ID_SEND_KEYSTATE,
    MSG_ID_FORCE_UINT16 = UINT16_MAX,
} pkt_messages;

typedef struct pkt_header {
    uint16_t id;
    uint16_t sender;
    uint16_t view_id;
    uint8_t *data;
    uint32_t datalen;
    int8_t is_reliable;
    int8_t ok;
    void* udata;
} pkt_header;

#define PKT_HANDLER_PROC(name) int32_t name(pkt_header *header)
typedef PKT_HANDLER_PROC(pkt_handler_proc);

typedef struct {
    uint16_t id;
    pkt_handler_proc *handler;
} pkt_handler;

int32_t pkt_header_encode(pkt_messages id, uint16_t view_id, void *data, size_t datalen);
int32_t pkt_header_decode(pkt_header *table, void *data, size_t datalen);

extern pkt_handler pkt_handlers[];
extern uint8_t pkt_buffer[];
