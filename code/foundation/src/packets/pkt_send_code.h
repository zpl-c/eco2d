#pragma once
#include "platform/system.h"
#include "pkt/packet_utils.h"

typedef struct {
	uint64_t code;
	uint32_t params[4];
	char data[128];
} pkt_send_code;

size_t pkt_code_send(uint64_t peer_id, uint16_t view_id, pkt_send_code code_data);
extern pkt_desc pkt_send_code_desc[];

PKT_HANDLER_PROC(pkt_send_code_handler);

