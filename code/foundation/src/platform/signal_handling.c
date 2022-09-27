#include <signal.h>
#include "zpl.h"
#include "core/game.h"
#ifdef ZPL_SYSTEM_WINDOWS
#include <Windows.h>

static BOOL WINAPI _sighandler_win32_control_handler(DWORD control_type)
{
    switch (control_type)
    {
        case CTRL_C_EVENT:
        case DBG_CONTROL_C:
        game_request_close();
        return 0;
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_SHUTDOWN_EVENT:
        game_request_close();
        return 1;
    }
    
    return 0;
}
#else //POSIX complaint
#include <sys/types.h>
static void _sighandler_posix_signal_handler(int sig) {
    (void)sig;
    game_request_close();
}
#endif

void sighandler_register() {
#ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(_sighandler_win32_control_handler, 1)) {
            zpl_printf("Could not set up signal handler!\n");
        }
    }
#else // POSIX compliant
    signal(SIGINT, &_sighandler_posix_signal_handler);
    signal(SIGTERM, &_sighandler_posix_signal_handler);
#endif
}

void sighandler_unregister() {
#ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(_sighandler_win32_control_handler, 0)) {
            zpl_printf("Could not uninstall signal handler!");
        }
    }
#else // POSIX compliant
#endif
}
