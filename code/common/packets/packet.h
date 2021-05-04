#pragma once
#include "system.h"

#define PKT_BUFSIZ 4096

typedef enum {
    MSG_ID_01_WELCOME,
    MSG_ID_LIBRG_UPDATE,
    MSG_ID_FORCE_UINT16 = UINT16_MAX,
} pkt_messages;

typedef struct pkt_header {
    uint16_t id;
    uint16_t sender;
    uint8_t *data;
    uint32_t datalen;
    int8_t ok;
} pkt_header;

#define PKT_HANDLER_PROC(name) int32_t name(pkt_header *header)
typedef PKT_HANDLER_PROC(pkt_handler_proc);

typedef struct {
    uint16_t id;
    pkt_handler_proc *handler;
} pkt_handler;

int32_t pkt_header_decode(pkt_header *table, void *data, size_t datalen);

extern pkt_handler pkt_handlers[];
extern uint8_t pkt_buffer[];
extern uint8_t pkt_pack_buffer[];

#include "packet_list.h"