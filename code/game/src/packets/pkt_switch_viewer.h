#pragma once
#include "system.h"
#include "packet_utils.h"

typedef struct {
    uint16_t view_id;
} pkt_switch_viewer;

size_t pkt_switch_viewer_send(uint16_t view_id);
extern pkt_desc pkt_switch_viewer_desc[];

PKT_HANDLER_PROC(pkt_switch_viewer_handler);

