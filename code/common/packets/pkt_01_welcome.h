#pragma once
#include "packet_utils.h"

typedef struct {
    uint16_t block_size;
    uint16_t chunk_size;
    uint16_t world_size;
} pkt_01_welcome;

size_t pkt_01_welcome_encode(pkt_01_welcome *table);
extern pkt_desc pkt_01_welcome_desc[];

PKT_HANDLER_PROC(pkt_01_welcome_handler);

