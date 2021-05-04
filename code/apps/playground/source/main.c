#define ZPL_IMPL
#define ZPL_NANO
#include <zpl.h>

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#include "librg.h"

#include "packets/packet.h"

int32_t mock_pkt_decode(pkt_desc *desc, uint32_t args, size_t msg_size, void *data, uint32_t size) {
    pkt_header header = {
        .data = pkt_buffer,
        .datalen = msg_size
    };
    return pkt_msg_decode(&header, desc, args, data, size);
}

int main(void) {
    pkt_01_welcome test_data = {
        .chunk_size = 4,
        .world_size = 8
    };
    
    size_t msg_size = pkt_01_welcome_encode(&test_data);
    pkt_01_welcome resp = {0};
    if (mock_pkt_decode(pkt_01_welcome_desc, 3, msg_size, PKT_STRUCT_PTR(&resp))) {
        zpl_printf("[err] oopsie!\n");
    }
    
    zpl_printf("size %d, amt %d, struct %d msg %d\n", resp.chunk_size, resp.world_size, sizeof(pkt_01_welcome), msg_size);
    
    return 0;
}
