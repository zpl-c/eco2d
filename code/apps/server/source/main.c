#define ZPL_IMPL
#include "zpl.h"

#include "system.h"
#include "network.h"
#include "world/world.h"
#include "utils/options.h"

#define DEFAULT_WORLD_SEED 302097
#define DEFAULT_CHUNK_SIZE 16
#define DEFAULT_CHUNK_AMOUNT 8

#define IF(call) do { \
    if (call != 0) { \
        zpl_printf("[ERROR] A call to a function %s failed\n", #call); \
        return 1; \
    } \
} while (0)

int main(int argc, char** argv) {
    zpl_opts opts={0};
    zpl_opts_init(&opts, zpl_heap(), argv[0]);

    zpl_opts_add(&opts, "?", "help", "the HELP section", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "p", "preview-map", "draw world preview", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "s", "seed", "world seed", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "r", "random-seed", "generate random world seed", ZPL_OPTS_FLAG);
    zpl_opts_add(&opts, "cs", "chunk-size", "size of a single chunk", ZPL_OPTS_INT);
    zpl_opts_add(&opts, "ca", "chunk-amount", "amount of chunks", ZPL_OPTS_INT);
    uint32_t ok = zpl_opts_compile(&opts, argc, argv);

    if (!ok) {
        zpl_opts_print_errors(&opts);
        zpl_opts_print_help(&opts);
        return -1;
    }
    int32_t seed = zpl_opts_integer(&opts, "seed", DEFAULT_WORLD_SEED);
    int32_t chunk_size = zpl_opts_integer(&opts, "chunk-size", DEFAULT_CHUNK_SIZE);
    int32_t chunk_amount = zpl_opts_integer(&opts, "chunk-amount", DEFAULT_CHUNK_AMOUNT);
    int32_t world_size = chunk_size * chunk_amount;

    if (zpl_opts_has_arg(&opts, "random-seed")) {
        zpl_random rnd={0};
        zpl_random_init(&rnd);
        seed = zpl_random_gen_u32(&rnd);
        zpl_printf("Seed: %u\n", seed);
    }

    if (zpl_opts_has_arg(&opts, "preview-map")) {
        generate_minimap(seed, world_size);
        return 0;
    }

    zpl_printf("[INFO] Generating world of size: %d x %d\n", world_size, world_size);
    IF(world_init(seed, world_size, world_size));

    zpl_printf("[INFO] Initializing network...\n");
    IF(network_init());
    IF(network_server_start("0.0.0.0", 27000));

    while (true) {
        network_server_tick();
    }

    IF(network_server_stop());
    IF(network_destroy());
    IF(world_destroy());

    return 0;
}
