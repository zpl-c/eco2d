#pragma once
#include "platform/system.h"
#include "raylib.h"
#include "world/blocks.h"
#include "models/assets.h"

Texture2D texgen_build_anim(asset_id id, int64_t counter);
Texture2D texgen_build_sprite(asset_id id);

// NOTE(zak): this is a fallback for when the asset is not defined by the game
Texture2D texgen_build_anim_fallback(asset_id id, int64_t counter);
Texture2D texgen_build_sprite_fallback(asset_id id);
