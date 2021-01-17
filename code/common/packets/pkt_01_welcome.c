// #include "packet.h"

// PACKET_GENERATE_ENCODE(1, 2, )

#include "cwpack/cwpack.h"

#define PKT_01_WELCOME_ID 1
#define PKT_01_WELCOME_ARGS 2

size_t PKT_01_welcome_encode(uint32_t chunk_size, uint32_t chunk_amount) {
    char buffer[20] = {0};

    cw_pack_context pc = {0};
    cw_pack_context_init(&pc, buffer, 20, 0);
    cw_pack_array_size(&pc, 1 + PKT_01_WELCOME_ARGS);
    cw_pack_signed(&pc, PKT_01_WELCOME_ID);

    cw_pack_unsigned(&pc, chunk_size);
    cw_pack_unsigned(&pc, chunk_amount);

    return pc.current - pc.start; /* length */
}
