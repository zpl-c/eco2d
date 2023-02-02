#pragma once
#include "platform/system.h"
#include "pkt/packet_utils.h"

typedef struct {
	char title[64];
	char text[1024];
} pkt_send_notification;

size_t pkt_notification_send(uint64_t peer_id, uint16_t view_id, const char *title, const char *text);
extern pkt_desc pkt_send_notification_desc[];

PKT_HANDLER_PROC(pkt_send_notification_handler);

