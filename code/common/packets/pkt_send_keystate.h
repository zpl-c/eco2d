#pragma once
#include "system.h"
#include "packet_utils.h"

typedef struct {
    double x;
    double y;
    uint8_t use;
} pkt_send_keystate;
size_t pkt_send_keystate_send(uint16_t view_id,
                              double x,
                              double y,
                              uint8_t use);
size_t pkt_send_keystate_encode(pkt_send_keystate *table);
pkt_desc pkt_send_keystate_desc[];

PKT_HANDLER_PROC(pkt_send_keystate_handler);

