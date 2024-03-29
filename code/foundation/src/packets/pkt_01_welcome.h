#pragma once
#include "platform/system.h"
#include "pkt/packet_utils.h"

typedef struct {
    uint32_t seed;
    uint64_t ent_id;
    uint16_t chunk_size;
    uint16_t world_size;
} pkt_01_welcome;

size_t pkt_01_welcome_send(uint32_t seed,
                           uint64_t peer_id,
                           uint16_t view_id,
                           uint64_t ent_id,
                           uint16_t chunk_size,
                           uint16_t world_size);
extern pkt_desc pkt_01_welcome_desc[];

PKT_HANDLER_PROC(pkt_01_welcome_handler);

