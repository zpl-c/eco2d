#pragma once
#include "system.h"
#include "packet_utils.h"

size_t pkt_send_librg_update_encode(void *data, int32_t data_length, uint8_t layer_id);

PKT_HANDLER_PROC(pkt_send_librg_update_handler);

