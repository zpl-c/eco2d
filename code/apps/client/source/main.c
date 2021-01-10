#include "raylib.h"
#include "game.h"

int main(void)
{
    game_init();

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        game_input();
        game_update();
        game_render();
    }

    game_shutdown();
    return 0;
}
