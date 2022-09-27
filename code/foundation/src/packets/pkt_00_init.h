#pragma once
#include "platform/system.h"
#include "pkt/packet_utils.h"

typedef struct {
    uint16_t view_id;
} pkt_00_init;

size_t pkt_00_init_send(uint16_t view_id);
extern pkt_desc pkt_00_init_desc[];

PKT_HANDLER_PROC(pkt_00_init_handler);

