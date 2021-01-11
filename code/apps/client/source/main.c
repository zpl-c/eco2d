#include "game.h"

int main(void)
{
    game_init();

    while (!WindowShouldClose())
    {
        game_input();
        game_update();
        game_render();
    }

    game_shutdown();
    return 0;
}
