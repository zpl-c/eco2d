#pragma once
#include "platform/system.h"
#include "pkt/packet_utils.h"

size_t pkt_send_librg_update(uint64_t peer_id,
                              uint16_t view_id,
                              uint8_t ticker,
                              void *data,
                              size_t datalen);
size_t pkt_send_librg_update_encode(void *data, int32_t data_length, uint8_t layer_id);

PKT_HANDLER_PROC(pkt_send_librg_update_handler);

