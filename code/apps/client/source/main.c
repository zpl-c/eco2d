#define ZPL_IMPL
#include "zpl.h"
#include "game.h"

int main(void)
{
    game_init();

    while (game_is_running())
    {
        game_input();
        game_update();
        game_render();
    }

    game_shutdown();
    return 0;
}
