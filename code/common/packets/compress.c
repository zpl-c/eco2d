#include "compress.h"

uint32_t compress_rle(void* data, uint32_t size, uint8_t *dest) {
    if (size < 1) return 0;
    uint32_t total_size = 0;
    uint8_t *buf = (uint8_t*)data;
    uint8_t byte = buf[0];
    uint16_t occurences = 1;
    for (uint32_t i = 1; i <= size; i += 1){
        if (buf[i] != byte || i == size) {
            *(uint16_t*)dest = occurences; dest += 2;
            *dest++ = byte;
            byte = buf[i];
            occurences = 1;
            total_size += 3;
        }
        else occurences++;
    }
    return total_size;
}

uint32_t decompress_rle(void* data, uint32_t size, uint8_t *dest) {
    if (size < 1) return 0;
    uint32_t total_size = 0;
    uint8_t *buf = (uint8_t*)data;
    for (uint32_t i = 0; i < size; i += 3){
        uint16_t len = *(uint16_t*)&buf[i];
        for (uint16_t j = 0; j < len; j += 1){
            *dest++ = buf[i+2];
            total_size++;
        }
    }
    return total_size;
}
