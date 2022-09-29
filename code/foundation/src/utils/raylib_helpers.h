#pragma once
#include "platform/system.h"
#include "raylib.h"
#include "world/blocks.h"
#include "gen/assets.h"

#define RAYLIB_NEW_RLGL
#include "rlgl.h"

static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }

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
