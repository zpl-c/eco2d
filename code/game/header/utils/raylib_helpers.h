#pragma once
#include "system.h"
#include "raylib.h"
#include "world/blocks.h"
#include "assets.h"

static inline 
void DrawTextEco(const char *text, float posX, float posY, int fontSize, Color color, float spacing) {
#if 1
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        Vector2 position = { (float)posX , (float)posY  };
        
        int defaultFontSize = 10;   // Default Font chars height in pixel
        int new_spacing = spacing == 0.0f ? fontSize/defaultFontSize : spacing;
        
        DrawTextEx(GetFontDefault(), text, position, (float)fontSize , (float)new_spacing , color);
    }
#endif
}

static inline 
int MeasureTextEco(const char *text, int fontSize, float spacing) {
#if 1
    Vector2 vec = { 0.0f, 0.0f };
    
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        int defaultFontSize = 10;   // Default Font chars height in pixel
        int new_spacing = spacing == 0.0f ? fontSize/defaultFontSize : spacing;
        
        vec = MeasureTextEx(GetFontDefault(), text, (float)fontSize, (float)new_spacing);
    }
    
    return (int)vec.x;
#else
    return 0;
#endif
}

static inline 
void DrawCircleEco(float centerX, float centerY, float radius, Color color)
{
    DrawCircleV((Vector2){ (float)centerX , (float)centerY  }, radius , color);
}

static inline 
void DrawRectangleEco(float posX, float posY, int width, int height, Color color)
{
    DrawRectangleV((Vector2){ (float)posX , (float)posY  }, (Vector2){ (float)width , (float)height  }, color);
}

static inline
Image GetBlockImage(uint8_t id) {
    return *(Image*)blocks_get_img(id);
}

static inline
Image GetSpriteImage(uint16_t id) {
    return *(Image*)assets_get_tex(id);
}

static inline
Sound GetSound(uint16_t id) {
    return *(Sound*)assets_get_snd(id);
}