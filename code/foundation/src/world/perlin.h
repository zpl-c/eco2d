#pragma once

#include "platform/system.h"

double perlin_noise2d(int32_t seed, double x, double y);
double perlin_fbm(int32_t seed, double x, double y, double freq, uint32_t octaves);
