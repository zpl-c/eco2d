#define ZPL_IMPL
#include "zpl.h"

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#include "librg.h"

#include "system.h"
#include "options.h"

#define DEFAULT_WORLD_SEED 302097

int main(int argc, char** argv) {
    zpl_opts opts={0};
    zpl_opts_init(&opts, zpl_heap(), argv[0]);

    zpl_opts_add(&opts, "?", "help", "the HELP section", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "p", "preview-map", "draw world preview", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "s", "seed", "world seed", ZPL_OPTS_INT);
    uint32_t ok = zpl_opts_compile(&opts, argc, argv);

    if (!ok) {
        zpl_opts_print_errors(&opts);
        zpl_opts_print_help(&opts);
        return -1;
    }
    int32_t seed = zpl_opts_integer(&opts, "seed", DEFAULT_WORLD_SEED);

    if (zpl_opts_has_arg(&opts, "preview-map")) {
        generate_minimap(seed);
        return 0;
    }

    printf("hello world\n");
    return 0;
}
