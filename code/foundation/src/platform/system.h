#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>

#define ZPL_NANO
#define ZPL_ENABLE_MATH
#include "zpl.h"

float get_cached_time(void);
void reset_cached_time(void);

#define defer_var ZPL_CONCAT(_i_,__LINE__)
#define defer(s,e) for ( \
uint32_t defer_var = (s, 0); \
!defer_var; \
(defer_var += 1), e)

#define PROT(...) __VA_ARGS__
