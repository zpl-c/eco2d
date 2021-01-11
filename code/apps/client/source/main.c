#define ZPL_IMPL
#include "zpl.h"
#include "game.h"
#include "signal_handling.h"

int main(void)
{
    sighandler_register();
    game_init();

    while (game_is_running())
    {
        game_input();
        game_update();
        game_render();
    }

    game_shutdown();
    sighandler_unregister();
    return 0;
}
