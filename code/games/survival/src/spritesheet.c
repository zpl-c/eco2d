typedef struct {
    Texture2D texture;
    Vector2 frameSize;
    int framesWide;
    Vector2 origin;
} SpriteSheet;

// enum {
//     SPRITE_PLAYER =  
// };

void sprite_draw(SpriteSheet* sprite, int frame, float x, float y, float ang, float scale, Color c) {
    float ox = (frame % sprite->framesWide) * sprite->frameSize.x;
    float oy = (int)(frame / sprite->framesWide) * sprite->frameSize.y;
    DrawTexturePro(sprite->texture, (Rectangle){ox, oy, sprite->frameSize.x,sprite->frameSize.y}, 
                                    (Rectangle){x, y, sprite->frameSize.x * scale, sprite->frameSize.y * scale}, 
                                    (Vector2){sprite->origin.x * scale, sprite->origin.y * scale}, ang, c);
}