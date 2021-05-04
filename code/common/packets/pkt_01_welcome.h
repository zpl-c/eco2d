#pragma once
#include "packet_utils.h"

#define PKT_01_NUMBERS_SIZ 32

typedef struct {
    uint32_t chunk_size;
    uint32_t chunk_amount;
    uint16_t numbers[PKT_01_NUMBERS_SIZ];
    pkt_header structs[64];
} pkt_01_welcome;

size_t pkt_01_welcome_encode(pkt_01_welcome *table);
extern pkt_desc pkt_01_welcome_desc[];

PKT_HANDLER_PROC(pkt_01_welcome_handler);

