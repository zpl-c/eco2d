#pragma once
#include "cwpack/cwpack.h"

uint32_t compress_rle(void* data, uint32_t size, uint8_t *dest);
uint32_t decompress_rle(void* data, uint32_t size, uint8_t *dest);
