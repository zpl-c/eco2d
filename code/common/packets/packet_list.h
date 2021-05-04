#pragma once

// NOTE(zaklaus): pkt data

typedef struct {
    uint32_t chunk_size;
    uint32_t chunk_amount;
} pkt_01_welcome;

// NOTE(zaklaus): pkt handlers

PKT_HANDLER_PROC(pkt_01_welcome_handler);

