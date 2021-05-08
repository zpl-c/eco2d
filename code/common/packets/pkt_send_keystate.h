#pragma once
#include "system.h"
#include "packet_utils.h"

typedef struct {
    float x;
    float y;
    uint8_t use;
    uint8_t sprint;
} pkt_send_keystate;
size_t pkt_send_keystate_send(uint16_t view_id,
                              float x,
                              float y,
                              uint8_t use,
                              uint8_t sprint);
size_t pkt_send_keystate_encode(pkt_send_keystate *table);
pkt_desc pkt_send_keystate_desc[];

PKT_HANDLER_PROC(pkt_send_keystate_handler);

