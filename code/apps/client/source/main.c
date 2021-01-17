#define ZPL_IMPL
#include "zpl.h"
#include "game.h"
#include "signal_handling.h"
#include "network.h"

int main(void)
{
    sighandler_register();
    game_init();
    network_init();
    network_client_connect("127.0.0.1", 27000);

    while (game_is_running())
    {
        game_input();
        game_update();
        game_render();
        network_client_tick();
    }

    network_client_disconnect();
    game_shutdown();
    sighandler_unregister();
    network_destroy();
    return 0;
}
