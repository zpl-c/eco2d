#pragma once
#include "system.h"
#include "raylib.h"
#include "world/blocks.h"
#include "assets.h"

#include "rlgl.h"

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
void DrawRectangleEco(float posX, float posY, float width, float height, Color color)
{
    DrawRectangleV((Vector2){ (float)posX , (float)posY  }, (Vector2){ width , height  }, color);
}

static inline
Texture2D GetBlockImage(uint8_t id) {
    return *(Texture2D*)blocks_get_img(id);
}

static inline
RenderTexture2D GetChunkTexture(uint64_t id) {
    RenderTexture2D *tex = (RenderTexture2D*)blocks_get_chunk_tex(id);
    if (!tex) return (RenderTexture2D){0};
    return *tex;
}

static inline
Image GetSpriteImage(uint16_t id) {
    return *(Image*)assets_get_tex(id);
}

static inline
Sound GetSound(uint16_t id) {
    return *(Sound*)assets_get_snd(id);
}

// Draw cube
// NOTE: Cube position is the center position
static inline
void EcoDrawCube(Vector3 position, float width, float height, float length, float heading, Color color)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    rlCheckRenderBatchLimit(36);
    
    rlPushMatrix();
    // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(heading, 0, 1, 0);
    //rlScalef(1.0f, 1.0f, 1.0f);   // NOTE: Vertices are directly scaled on definition
    
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    
    // Front face
    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
    
    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right
    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
    
    // Back face
    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left
    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
    
    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
    
    // Top face
    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Bottom Left
    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right
    
    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left
    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right
    
    // Bottom face
    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left
    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
    
    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Right
    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left
    
    // Right face
    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
    rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right
    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left
    
    rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left
    rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right
    rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left
    
    // Left face
    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
    rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Right
    
    rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left
    rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left
    rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right
    rlEnd();
    rlPopMatrix();
}
