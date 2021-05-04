#define ZPL_IMPL
#define ZPL_NANO
#include <zpl.h>

#include "mock.h"
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
        .chunk_size = world_chunk_size(),
        .chunk_amount = world_chunk_amount()
    };
    
    for (int i = 0; i < 4; i += 1){
        test_data.numbers[i] = i*2;
    }
    
    size_t msg_size = pkt_01_welcome_encode(&test_data);
    pkt_01_welcome resp = {0};
    if (mock_pkt_decode(pkt_01_welcome_desc, 3, msg_size, PKT_STRUCT_PTR(&resp))) {
        zpl_printf("[err] oopsie!\n");
    }
    
    zpl_printf("size %d, amt %d, num[1] %d struct %d msg %d\n", resp.chunk_size, resp.chunk_amount, resp.numbers[1], sizeof(pkt_01_welcome), msg_size);
    
    return 0;
}
