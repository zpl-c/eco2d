#pragma once
#include "system.h"
#include "raylib.h"
#include "world/blocks.h"
#include "assets.h"

#define RAYLIB_NEW_RLGL
#include "rlgl.h"

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

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
Texture2D GetSpriteTexture2D(uint16_t id) {
    return *(Texture2D*)assets_get_tex(id);
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

// Draw codepoint at specified position in 3D space
void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint)
{
#if 0
    // Character index position in sprite font
    // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scale = fontSize/(float)font.baseSize;
    
    // Character destination rectangle on screen
    // NOTE: We consider charsPadding on drawing
    position.x += (float)(font.chars[index].offsetX - font.charsPadding)/(float)font.baseSize*scale;
    position.z += (float)(font.chars[index].offsetY - font.charsPadding)/(float)font.baseSize*scale;
    
    // Character source rectangle from font texture atlas
    // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.charsPadding, font.recs[index].y - (float)font.charsPadding,
        font.recs[index].width + 2.0f*font.charsPadding, font.recs[index].height + 2.0f*font.charsPadding };
    
    float width = (float)(font.recs[index].width + 2.0f*font.charsPadding)/(float)font.baseSize*scale;
    float height = (float)(font.recs[index].height + 2.0f*font.charsPadding)/(float)font.baseSize*scale;
    
    if (font.texture.id > 0)
    {
        const float x = 0.0f;
        const float y = 0.0f;
        const float z = 0.0f;
        
        // normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
        const float tx = srcRec.x/font.texture.width;
        const float ty = srcRec.y/font.texture.height;
        const float tw = (srcRec.x+srcRec.width)/font.texture.width;
        const float th = (srcRec.y+srcRec.height)/font.texture.height;
        
        {
#if defined(RAYLIB_NEW_RLGL)
        }
        rlCheckRenderBatchLimit(4 + 4*backface);
        rlSetTexture(font.texture.id);
#else
        if (rlCheckBufferLimit(4 + 4*backface)) rlglDraw();
        rlEnableTexture(font.texture.id);
#endif
        rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        
        rlBegin(RL_QUADS);
        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        
        // Front Face
        rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
        rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);              // Top Left Of The Texture and Quad
        rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height);     // Bottom Left Of The Texture and Quad
        rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);     // Bottom Right Of The Texture and Quad
        rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad
        
        if (backface)
        {
            // Back Face
            rlNormal3f(0.0f, -1.0f, 0.0f);                              // Normal Pointing Down
            rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);          // Top Right Of The Texture and Quad
            rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);          // Top Left Of The Texture and Quad
            rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height); // Bottom Left Of The Texture and Quad
            rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height); // Bottom Right Of The Texture and Quad
        }
        rlEnd();
        rlPopMatrix();
        
#if defined(RAYLIB_NEW_RLGL)
        rlSetTexture(0);
#else
        rlDisableTexture();
#endif
    }
#endif
}

void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint) {
#if 0
    int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop
    
    float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;               // Offset X to next character to draw
    
    float scale = fontSize/(float)font.baseSize;
    
    for (int i = 0; i < length;)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);
        
        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        
        if (codepoint == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += scale + lineSpacing/(float)font.baseSize*scale;
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint3D(font, codepoint, (Vector3){ position.x + textOffsetX, position.y, position.z + textOffsetY }, fontSize, backface, tint);
            }
            
            if (font.chars[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing)/(float)font.baseSize*scale;
            else textOffsetX += (float)(font.chars[index].advanceX + fontSpacing)/(float)font.baseSize*scale;
        }
        
        i += codepointByteCount;   // Move text bytes counter to next codepoint
        
    }
#endif
}

Vector3 MeasureText3D(Font font, const char* text, float fontSize, float fontSpacing, float lineSpacing) {
    
#if 0    
    int len = TextLength(text);
    int tempLen = 0;                // Used to count longer text line num chars
    int lenCounter = 0;
    
    float tempTextWidth = 0.0f;     // Used to count longer text line width
    
    float scale = fontSize/(float)font.baseSize;
    float textHeight = scale;
    float textWidth = 0.0f;
    
    int letter = 0;                 // Current character
    int index = 0;                  // Index position in sprite font
    
    for (int i = 0; i < len; i++)
    {
        lenCounter++;
        
        int next = 0;
        letter = GetCodepoint(&text[i], &next);
        index = GetGlyphIndex(font, letter);
        
        // NOTE: normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) next = 1;
        i += next - 1;
        
        if (letter != '\n')
        {
            if (font.chars[index].advanceX != 0) textWidth += (font.chars[index].advanceX+fontSpacing)/(float)font.baseSize*scale;
            else textWidth += (font.recs[index].width + font.chars[index].offsetX)/(float)font.baseSize*scale;
        }
        else
        {
            if (tempTextWidth < textWidth) tempTextWidth = textWidth;
            lenCounter = 0;
            textWidth = 0.0f;
            textHeight += scale + lineSpacing/(float)font.baseSize*scale;
        }
        
        if (tempLen < lenCounter) tempLen = lenCounter;
    }
    
    if (tempTextWidth < textWidth) tempTextWidth = textWidth;
    
    Vector3 vec = { 0 };
    vec.x = tempTextWidth + (float)((tempLen - 1)*fontSpacing/(float)font.baseSize*scale); // Adds chars spacing to measure
    vec.y = 0.25f;
    vec.z = textHeight;
    
    return vec;
#endif
    
}


Color GenerateRandomColor(float s, float v) {
    const float Phi = 0.618033988749895f; // Golden ratio conjugate
    float h = GetRandomValue(0, 360);
    h = fmodf((h + h*Phi), 360.0f);
    return ColorFromHSV(h, s, v);
}


// Draw circle outline
void DrawCircleLinesEco(float centerX, float centerY, float radius, Color color)
{
    rlCheckRenderBatchLimit(2*36);
    
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    
    // NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
    for (int i = 0; i < 360; i += 10)
    {
        rlVertex2f(centerX + sinf(DEG2RAD*i)*radius, centerY + cosf(DEG2RAD*i)*radius);
        rlVertex2f(centerX + sinf(DEG2RAD*(i + 10))*radius, centerY + cosf(DEG2RAD*(i + 10))*radius);
    }
    rlEnd();
}

void DrawRectangleLinesEco(float posX, float posY, float width, float height, Color color)
{
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(posX + 1, posY + 1);
    rlVertex2f(posX + width, posY + 1);
    
    rlVertex2f(posX + width, posY + 1);
    rlVertex2f(posX + width, posY + height);
    
    rlVertex2f(posX + width, posY + height);
    rlVertex2f(posX + 1, posY + height);
    
    rlVertex2f(posX + 1, posY + height);
    rlVertex2f(posX + 1, posY + 1);
    rlEnd();
}
