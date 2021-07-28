#pragma once
#include "system.h"
#include "raylib.h"
#include "world/blocks.h"
#include "assets.h"

Texture2D texgen_build_block(uint32_t biome, uint32_t kind);
Texture2D texgen_build_sprite(asset_id id);
