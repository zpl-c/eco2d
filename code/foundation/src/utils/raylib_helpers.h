#pragma once
#include <math.h>
#include "platform/system.h"
#include "raylib.h"
#include "raymath.h"
#include "world/blocks.h"
#include "models/assets.h"

#define RAYLIB_NEW_RLGL
#include "rlgl.h"

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

static inline
Color BlendColor(Color a, Color b, float t) {
    return (Color) {
        .r = (uint8_t)(lerp((float)(a.r)/255.0f, (float)(b.r)/255.0f, t) * 255),
        .g = (uint8_t)(lerp((float)(a.g)/255.0f, (float)(b.g)/255.0f, t) * 255),
        .b = (uint8_t)(lerp((float)(a.b)/255.0f, (float)(b.b)/255.0f, t) * 255),
        .a = (uint8_t)(lerp((float)(a.a)/255.0f, (float)(b.a)/255.0f, t) * 255),
    };
}

static inline
Texture2D LoadTexEco(const char *name) {
    static char filename[128];
    zpl_snprintf(filename,  128, "art/gen/%s.png", name);
    return LoadTexture(filename);
}

static inline
Image LoadImageEco(const char *name) {
    static char filename[128];
    zpl_snprintf(filename,  128, "art/gen/%s.png", name);
    return LoadImage(filename);
}

static inline
Texture2D Image2TexEco(Image image) {
    Texture2D tex = LoadTextureFromImage(image);
    UnloadImage(image);
    return tex;
}

static inline
Texture2D GenColorEco(Color color) {
    Image img = GenImageColor(1, 1, color);
    return Image2TexEco(img);
}

static inline
Texture2D GenFrameRect() {
    RenderTexture2D temp_texture = LoadRenderTexture(64, 64);

    Color mouse_color_a = {0, 0, 0, 200};
    Color mouse_color_b = {255, 255, 255, 200};

    BeginTextureMode(temp_texture);
        DrawRectangleLines(0, 0, 64, 64, mouse_color_a);
        DrawRectangleLines(1, 1, 62, 62, mouse_color_b);
    EndTextureMode();

    return temp_texture.texture;
}

static inline
void DrawTextEco(const char *text, float posX, float posY, float fontSize, Color color, float spacing) {
#if 1
    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        Vector2 position = { posX , posY  };

        float defaultFontSize = 10.0;   // Default Font chars height in pixel
        float new_spacing = spacing == 0.0f ? fontSize/defaultFontSize : spacing;

        DrawTextEx(GetFontDefault(), text, position, fontSize , new_spacing , color);
    }
#endif
}

static inline
float MeasureTextEco(const char *text, float fontSize, float spacing) {
#if 1
    Vector2 vec = { 0.0f, 0.0f };

    // Check if default font has been loaded
    if (GetFontDefault().texture.id != 0) {
        float defaultFontSize = 10.0;   // Default Font chars height in pixel
        float new_spacing = spacing == 0.0f ? fontSize/defaultFontSize : spacing;

        vec = MeasureTextEx(GetFontDefault(), text, fontSize, (float)new_spacing);
    }

    return vec.x;
#else
    return 0.f;
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
Texture2D GetBlockImage(block_id id) {
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

static inline
Color GenerateRandomColor(float s, float v) {
    const float Phi = 0.618033988749895f; // Golden ratio conjugate
    float h = (float)GetRandomValue(0, 360);
    h = fmodf((h + h*Phi), 360.0f);
    return ColorFromHSV(h, s, v);
}


// Draw circle outline
static inline
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

static inline
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

static inline
void DrawSpriteTextureEco(Texture2D texture, Vector3 position, float width, float height, float length, Color color) {
    float x = position.x;
    float y = position.y;
    float z = position.z;

    Vector3 rotationAxis = {1.0f, 0.f, 0.f};

    // NOTE: Plane is always created on XZ ground
    rlSetTexture(texture.id);

    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(25.f, rotationAxis.x, rotationAxis.y, rotationAxis.z);
        rlScalef(width, height, length);

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);

            // Top Face
            rlNormal3f(0.0f, 1.0f, 0.0f);                  // Normal Pointing Up
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-0.5f, 0.0f, -0.5f);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-0.5f, 0.0f, 0.5f);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(0.5f, 0.0f, 0.5f);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(0.5f, 0.0f, -0.5f);  // Top Right Of The Texture and Quad
        rlEnd();
    rlPopMatrix();

    rlSetTexture(0);
}

static inline
void Draw3DRectangle(Camera camera, float x, float y, float z, float w, float h, Color tint)
{
    rlPushMatrix();

    // get the camera view matrix
    Matrix mat = MatrixLookAt(camera.position, camera.target, camera.up);
    // peel off just the rotation
    Quaternion quat = QuaternionFromMatrix(mat);
    mat = QuaternionToMatrix(quat);

    // apply just the rotation
    rlMultMatrixf(MatrixToFloat(mat));

    Vector3 position = (Vector3){x,z,y};
    position = Vector3Transform(position, MatrixInvert(mat));
    rlTranslatef(position.x, position.y, position.z);

    // draw the billboard
    float width = w / 2;
    float height = h / 2;

    rlCheckRenderBatchLimit(6);

    // draw quad
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
    // Front Face
    rlNormal3f(0.0f, 0.0f, 1.0f);                  // Normal Pointing Towards Viewer
    rlVertex3f(-width, -height, 0);  // Bottom Left Of The Texture and Quad
    rlVertex3f(+width, -height, 0);  // Bottom Right Of The Texture and Quad
    rlVertex3f(+width, +height, 0);  // Top Right Of The Texture and Quad
    rlVertex3f(-width, +height, 0);  // Top Left Of The Texture and Quad

    rlEnd();
    rlPopMatrix();
}

static inline
void Draw3DBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint)
{
    rlPushMatrix();

    // get the camera view matrix
    Matrix mat = MatrixLookAt(camera.position, camera.target, camera.up);
    // peel off just the rotation
    Quaternion quat = QuaternionFromMatrix(mat);
    mat = QuaternionToMatrix(quat);

    // apply just the rotation
    rlMultMatrixf(MatrixToFloat(mat));

    // translate backwards in the inverse rotated matrix to put the item where it goes in world space
    position = Vector3Transform(position, MatrixInvert(mat));
    rlTranslatef(position.x, position.y, position.z);

    // draw the billboard
    float width = size.x / 2;
    float height = size.y / 2;

    rlCheckRenderBatchLimit(6);

    rlSetTexture(texture.id);

    // draw quad
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
    // Front Face
    rlNormal3f(0.0f, 0.0f, 1.0f);                  // Normal Pointing Towards Viewer

    rlTexCoord2f((float)source.x / texture.width, (float)(source.y + source.height) / texture.height);
    rlVertex3f(-width, -height, 0);  // Bottom Left Of The Texture and Quad

    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)(source.y + source.height) / texture.height);
    rlVertex3f(+width, -height, 0);  // Bottom Right Of The Texture and Quad

    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)source.y / texture.height);
    rlVertex3f(+width, +height, 0);  // Top Right Of The Texture and Quad

    rlTexCoord2f((float)source.x / texture.width, (float)source.y / texture.height);
    rlVertex3f(-width, +height, 0);  // Top Left Of The Texture and Quad

    rlEnd();
    rlSetTexture(0);
    rlPopMatrix();
}

static inline
void Draw3DBillboard(Camera camera, Texture2D texture, Vector3 position, float size, Color tint)
{
    Draw3DBillboardRec(camera, texture, (Rectangle){ 0,0,(float)texture.width,(float)texture.height }, position, (Vector2){ size,size }, tint);
}

// Draw codepoint at specified position in 3D space
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint)
{
    // Character index position in sprite font
    // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scale = fontSize/(float)font.baseSize;

    // Character destination rectangle on screen
    // NOTE: We consider charsPadding on drawing
    position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding)/(float)font.baseSize*scale;
    position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding)/(float)font.baseSize*scale;

    // Character source rectangle from font texture atlas
    // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f*font.glyphPadding, font.recs[index].height + 2.0f*font.glyphPadding };

    float width = (float)(font.recs[index].width + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;
    float height = (float)(font.recs[index].height + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;

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

        rlCheckRenderBatchLimit(4 + 4*backface);
        rlSetTexture(font.texture.id);

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

        rlSetTexture(0);
    }
}

// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint)
{
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
            textOffsetY += scale + lineSpacing/(float)font.baseSize*scale;
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint3D(font, codepoint, (Vector3){ position.x + textOffsetX, position.y, position.z + textOffsetY }, fontSize, backface, tint);
            }

            if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing)/(float)font.baseSize*scale;
            else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing)/(float)font.baseSize*scale;
        }

        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }
}

// Measure a text in 3D. For some reason `MeasureTextEx()` just doesn't seem to work so i had to use this instead.
static Vector3 MeasureText3D(Font font, const char* text, float fontSize, float fontSpacing, float lineSpacing)
{
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
            if (font.glyphs[index].advanceX != 0) textWidth += (font.glyphs[index].advanceX+fontSpacing)/(float)font.baseSize*scale;
            else textWidth += (font.recs[index].width + font.glyphs[index].offsetX)/(float)font.baseSize*scale;
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
}
