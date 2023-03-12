#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/// lib.c

// core

typedef int efd_app;
typedef uint16_t efd_entity_type;
typedef int32_t efd_result;
typedef void * efd_world;
typedef int efd_entid;

// define amount of memeory reserved for???
// custom data within COMMAND (cli->ser) and SNAPSHOT (ser->cli)
#define EFD_COMMAND_SIZE 64 * 1024
#define EFD_SNAPSHOT_SIZE 128 * 1024

#define EFD_TYPE_SHIFT 16
#define EFD_ID(type) (((type) << EFD_TYPE_SHIFT) + 1)
#define EFD_TYPE(id) ((id - 1) >> EFD_TYPE_SHIFT)
#define EFD_END 0

typedef enum efd_instance_type {
    EFD_TYPE_NONE = 0,

    /* chunk */
    EFD_TYPE_TILE,
    EFD_TYPE_BLOCK,
    EFD_TYPE_CHUNK,

    /* entities */
    EFD_TYPE_ITEM,
    EFD_TYPE_OBJECT,
    EFD_TYPE_DEVICE,
    EFD_TYPE_VEHICLE,
    EFD_TYPE_MOB,
    EFD_TYPE_PLAYER,

    /* other */
    EFD_TYPE_OTHER,

    /* utils */
    EFD_TYPE_LAST,
    EFD_TYPE_MAX = 0xFFFF,
} efd_instance_type;

typedef enum efd_flags {
    EFD_FLAG_COLLISION = (1 << 1),
    EFD_FLAG_HAZARD = (1 << 2),
    EFD_FLAG_ESSENTIAL = (1 << 3),
    EFD_FLAG_DESTROY_ON_COLLISION = (1 << 4),
} efd_flags;


// assets

typedef enum efd_asset_type {
    EFD_NONE = 0,
    EFD_TEXTURE,
    EFD_ANIMATION,
    EFD_SOUND,
    EFD_FONT,
    EFD_SHADER,
} efd_asset_type;

typedef enum efd_asset_flags {
    EFD_FLAG_ATLAS = (1 << 1),   // texture is an atlas and has multiple frames
    EFD_FLAG_LOOPED = (1 << 2),  // frames are looped sequentially (requrires EFD_FLAG_ATLAS, can be combined with EFD_FLAG_RANDOM)
    EFD_FLAG_RANDOM = (1 << 3),  // one of the frames is randomly chosen (requrires EFD_FLAG_ATLAS, can be combined with EFD_FLAG_LOOPED)
    EFD_FLAG_COMPLEX = (1 << 4), // frames have a complex state system and require a separate asset with EFD_ANIMATION type
} efd_asset_flags;

typedef struct efd_asset {
    efd_asset_type type;
    int id;
    const char *path;
    int32_t flags;
    struct {
        int size_x;
        int size_y;
    } atlas;
    void *data;
} efd_asset;

// objects

typedef struct efd_tile {
    int id;
    int flags;
    float drag;
    float friction;
} efd_tile;

typedef struct efd_block {
    int id;
    int flags;
    float bounce;
    int velx;
    int vely;
} efd_block;

typedef struct efd_item_list {
    int id;
    int qty;
} efd_item_list;

typedef struct efd_craft {
    int producer;
    int ticks;
    efd_item_list *input;
    efd_item_list *output;
} efd_craft;

typedef struct efd_tooltip {
    const char *title;
    const char *description;
} efd_tooltip;

typedef struct efd_entity {
    int id;
    int flags;
    int stack;
    union {
        int durability;
        int health;
    };
    efd_result (*spawn_cb)(efd_entid);
    efd_item_list *drop;
} efd_entity;


typedef struct efd_app_desc {
    const char *name;
    int debug_ui;

    int width;
    int height;
    int fullscreen;
    int vsync;

    int server_only;
    int viewer_only;
    int viewer_amount;
    int world_seed;
    int world_seed_random;

    efd_result (*init_cb)();
    efd_result (*system_cb)();
    efd_result (*update_cb)();
    efd_result (*render_cb)(efd_entity_type type);
    efd_result (*player_join_cb)(efd_entid player_id);
    efd_result (*player_leave_cb)(efd_entid player_id);
    efd_result (*cleanup_cb)();

    struct {
        float item_pickup_radius;
        float item_merge_radius;
        float item_drop_radius;
        float item_attract_radius;
        float item_attract_force;
    } rules;

    efd_asset *assets;
    efd_tile *tiles;
    efd_block *blocks;
    efd_entity *entities;
    efd_craft *crafting;
    efd_tooltip *tooltips;
} efd_app_desc;

/// app.c

enum {
    /* textures */
    TILE_AIR = EFD_ID(EFD_TYPE_TILE),
    TILE_DIRT,
    TILE_GRASS,
    TILE_STONE,
    TILE_WATER,
    TILE_LAVA,

    BLOCK_AIR = EFD_ID(EFD_TYPE_BLOCK),
    BLOCK_WOOD,
    BLOCK_HILL,
    BLOCK_HILL_SNOW,
    BLOCK_FENCE_LEFT,
    BLOCK_FENCE_RIGHT,
    BLOCK_FENCE_UP,
    BLOCK_FENCE_DOWN,
    BLOCK_WALL_LEFT,
    BLOCK_WALL_RIGHT,
    BLOCK_WALL_UP,
    BLOCK_WALL_DOWN,
    BLOCK_BELT_LEFT,
    BLOCK_BELT_RIGHT,
    BLOCK_BELT_UP,
    BLOCK_BELT_DOWN,

    ENTITY_IRON_ORE = EFD_ID(EFD_TYPE_ITEM),
    ENTITY_IRON_INGOT,
    ENTITY_IRON_PLATE,
    ENTITY_SCREW,
    ENTITY_BELT,

    ENTITY_FIR_TREE = EFD_ID(EFD_TYPE_OBJECT),
    ENTITY_OAK_TREE,

    ENTITY_CHEST = EFD_ID(EFD_TYPE_DEVICE),
    ENTITY_FURNACE,
    ENTITY_CRAFTBENCH,
    ENTITY_ASSEMBLER,

    ENTITY_MONSTER = EFD_ID(EFD_TYPE_MOB),
    ENTITY_RABBIT,
    ENTITY_BOAR,
    ENTITY_BIZON,
    ENTITY_BEAR,

    ENTITY_TRUCK = EFD_ID(EFD_TYPE_VEHICLE),
    ENTITY_CAR,

    ENTITY_PLAYER = EFD_ID(EFD_TYPE_PLAYER),
};

efd_result init() {
    return 0;
}

efd_result systems(efd_world *w) {
    // ECS_SYSTEM(w, Move, EcsOnUpdate, Position, [in] Velocity);

    return 0;
}

efd_result update() {
    return 0;
}

efd_result render(efd_entity_type type) {
    // switch (type) {
    //     case ENTITY_PLAYER:
    //         /* additional/replacable rendering code on top of what EFD provides for built-in types */
    //         efd_render_texture(ENTITY_PLAYER, 0, 0, 0, 0, 0, 0, 0, 0);

    //         return 1; /* we handled the rendering, don't render with the default renderer */
    //         break;

    //     case ENTITY_MONSTER:
    //         /* our custom rendering code for monster */
    //         efd_render_texture(ENTITY_MONSTER, 0, 0, 0, 0, 0, 0, 0, 0);
    //         break;

    //     case ENTITY_WEAPON:
    //         /* our custom rendering code for weapon */
    //         break;
    // }

    return 0;
}

efd_result player_join(efd_entid player) {
    // efd_notify_push(player, "Test", "Welcome to the game!", 5.0f);
    return 0;
}

efd_result player_leave(efd_entid player) {
    return 0;
}

efd_result cleanup() {
    return 0;
}

efd_result chest_spawn(efd_entid id) {
    return 0;
}

efd_result furnace_spawn(efd_entid id) {
    return 0;
}

efd_app_desc efd_main() {
    return (efd_app_desc){
        .name = "Survival",
        .debug_ui = true,

        /* these are defaults, with zpl_option/config files overrides */
        .width = 1280,
        .height = 720,
        .fullscreen = 0,
        .vsync = 1,

        /* these are code overrides for zpl_option flags (not defaults) */
        .server_only = false,
        .viewer_only = false,
        .viewer_amount = 1,
        .world_seed = 0,
        .world_seed_random = true,

        /* primary callbacks */
        .init_cb = init,
        .system_cb = systems,
        .update_cb = update,
        .render_cb = render,
        .player_join_cb = player_join,
        .player_leave_cb = player_leave,
        .cleanup_cb = cleanup,

        .rules = {
            .item_pickup_radius = 25.0f,
            .item_merge_radius = 75.0f,
            .item_drop_radius = 15.0f,
            .item_attract_radius = 75.0f,
            .item_attract_force = 0.1f,
        },

        .assets = (efd_asset[]){
            {EFD_TEXTURE, TILE_GRASS, "assets/grass.png", EFD_FLAG_ATLAS | EFD_FLAG_RANDOM, .atlas = {4, 1}},
            {EFD_TEXTURE, TILE_DIRT, "assets/dirt.png", EFD_FLAG_ATLAS | EFD_FLAG_RANDOM, .atlas = {4, 1}},
            {EFD_TEXTURE, TILE_LAVA, "assets/lava.png", EFD_FLAG_ATLAS | EFD_FLAG_LOOPED, .atlas = {4, 1}},
            {EFD_TEXTURE, TILE_WATER, "assets/water.png", EFD_FLAG_ATLAS | EFD_FLAG_RANDOM | EFD_FLAG_LOOPED, .atlas = {4, 1}},

            {EFD_TEXTURE, ENTITY_OAK_TREE, "assets/tree.png"},
            {EFD_TEXTURE, ENTITY_CHEST, "assets/chest.png"},
            {EFD_TEXTURE, ENTITY_FURNACE, "assets/furnace.png"},

            {EFD_TEXTURE, ENTITY_PLAYER, "assets/player.png", EFD_FLAG_ATLAS | EFD_FLAG_COMPLEX},
            {EFD_TEXTURE, ENTITY_MONSTER, "assets/monster.png"},

            /* animations */
            {EFD_ANIMATION, ENTITY_PLAYER, "assets/player.anim"},

            /* sounds */
            {EFD_SOUND, ENTITY_PLAYER, "assets/player.wav"},

            {EFD_END},
        },

        .tiles = (efd_tile[]){
            {TILE_AIR},
            {TILE_DIRT, .drag = 1.0f, .friction = 1.0f},
            {TILE_GRASS, .drag = 1.0f, .friction = 1.0f},
            {TILE_WATER, .drag = 0.11f , .friction = 10.0f},
            {TILE_LAVA, EFD_FLAG_HAZARD, .drag = 6.2f , .friction = 40.0f},

            {EFD_END},
        },

        .blocks = (efd_block[]){
            {BLOCK_AIR},
            {BLOCK_HILL, EFD_FLAG_COLLISION},
            {BLOCK_HILL_SNOW, EFD_FLAG_COLLISION},
            {BLOCK_WOOD, EFD_FLAG_COLLISION, .bounce = 0.0f},

            {BLOCK_WALL_LEFT, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_WALL_RIGHT, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_WALL_UP, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_WALL_DOWN, EFD_FLAG_COLLISION, .bounce = 1.0f},

            {BLOCK_FENCE_LEFT, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_FENCE_RIGHT, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_FENCE_UP, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_FENCE_DOWN, EFD_FLAG_COLLISION, .bounce = 1.0f},

            {BLOCK_BELT_LEFT, .velx = -150.0f},
            {BLOCK_BELT_RIGHT, .velx = 150.0f},
            {BLOCK_BELT_UP, .vely = -150.0f},
            {BLOCK_BELT_DOWN, .vely = 150.0f},

            {EFD_END},
        },

        .entities = (efd_entity[]){
            {ENTITY_IRON_ORE, .stack = 32},
            {ENTITY_IRON_PLATE, .stack = 32},
            {ENTITY_SCREW, .stack = 256},

            {ENTITY_OAK_TREE, EFD_FLAG_DESTROY_ON_COLLISION, .drop = (efd_item_list[]){
                {ENTITY_SCREW, 1},
                {EFD_END},
            }},

            {ENTITY_CHEST, .spawn_cb = chest_spawn},
            {ENTITY_FURNACE, .spawn_cb = furnace_spawn},

            {ENTITY_MONSTER, .health = 256},
            {ENTITY_TRUCK},
            {ENTITY_PLAYER},

            {EFD_END},
        },

        .crafting = (efd_craft[]){
            {
                .producer = ENTITY_FURNACE,
                .ticks = 20,
                .input = (efd_item_list[]){
                    {ENTITY_IRON_ORE, 1},
                    {EFD_END},
                },
                .output = (efd_item_list[]){
                    {ENTITY_IRON_PLATE, 4},
                    {EFD_END},
                },
            },
            {
                .producer = ENTITY_CRAFTBENCH,
                .ticks = 40,
                .input = (efd_item_list[]){
                    {ENTITY_IRON_PLATE, 1},
                    {EFD_END},
                },
                .output = (efd_item_list[]){
                    {ENTITY_SCREW, 8},
                    {EFD_END},
                },
            },
            {
                .producer = ENTITY_ASSEMBLER,
                .ticks = 120,
                .input = (efd_item_list[]){
                    {ENTITY_SCREW, 4},
                    {ENTITY_IRON_PLATE, 2},
                    {EFD_END},
                },
                .output = (efd_item_list[]){
                    {ENTITY_BELT, 1},
                    {EFD_END},
                },
            },

            {EFD_END},
        },

        .tooltips = (efd_tooltip[]){
            {"BLOCK_STONE", "It's a block of stone, what did you expect?"},
            {"BLOCK_BRICK", "It's a block of brick, what did you expect?"},
            {EFD_END},
        },
    };
}

/// test.c

#include <stdio.h>

void test(efd_entity_type type) { printf("type: %d\n", type); }

int main(int argc, char **argv) {
    efd_app_desc desc = efd_main();
    printf("value: %f\n", desc.rules.item_pickup_radius);
    printf("TILE_DIRT: %d\n", TILE_DIRT);
    printf("TILE_GRASS: %d\n", TILE_GRASS);
    printf("ENTITY_PLAYER: %d, is_valid type: %d\n", ENTITY_PLAYER, EFD_TYPE(ENTITY_PLAYER) == EFD_TYPE_PLAYER);
    printf("ENTITY_CHEST: %d, is_valid type: %d\n", ENTITY_CHEST, EFD_TYPE(ENTITY_CHEST) == EFD_TYPE_DEVICE);

    desc.init_cb();

    return 0;
}
