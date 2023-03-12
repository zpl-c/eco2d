#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/// lib.c

// core

typedef int efd_app;
typedef uint16_t efd_entity_type;
typedef int32_t efd_result;
typedef void * efd_world;

// define amount of memeory reserved for???
// custom data within COMMAND (cli->ser) and SNAPSHOT (ser->cli)
#define EFD_COMMAND_SIZE 64 * 1024
#define EFD_SNAPSHOT_SIZE 128 * 1024

#define EFD_TYPE_SHIFT 16
#define EFD_ID(type) (((type) << EFD_TYPE_SHIFT) + 1)
#define EFD_TYPE(id) ((id - 1) >> EFD_TYPE_SHIFT)
#define EFD_END 0

typedef enum efd_asset_type {
    EFD_NONE = 0,
    EFD_TEXTURE,
    EFD_TEXTURE_ATLAS,
    EFD_ANIMATION,
    EFD_SOUND,
    EFD_FONT,
    EFD_SHADER,
} efd_asset_type;

typedef enum efd_object_type {
    EFD_TYPE_NONE,
    EFD_TYPE_TILE,
    EFD_TYPE_BLOCK,
    EFD_TYPE_ENTITY,
    EFD_TYPE_ITEM,
    EFD_TYPE_CRAFT,
    EFD_TYPE_CHUNK,

    EFD_TYPE_TYPE_LAST,
    EFD_TYPE_TYPE_MAX = 0xFFFF,
} efd_object_type;

typedef enum efd_flags {
    EFD_FLAG_COLLISION = (1 << 1),
    EFD_FLAG_HAZARD = (1 << 2),
    EFD_FLAG_ESSENTIAL = (1 << 3),
    EFD_FLAG_DESTROY_ON_COLLISION = (1 << 4),
    EFD_FLAG_AI = (1 << 5),
    EFD_FLAG_PLAYER = (1 << 6),
    EFD_FLAG_VEHICLE = (1 << 7),
} efd_flags;

typedef struct efd_asset {
    efd_asset_type type;
    int id;
    const char *path;
    void *data; /* TODO: make a union */
} efd_asset;

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

typedef struct efd_entity {
    int id;
    int flags;
} efd_entity;

typedef struct efd_item {
    int id;
    const char *t1;
    const char *t2;
} efd_item;

typedef struct efd_craft_item {
    int id;
    int qty;
} efd_craft_item;

typedef struct efd_craft {
    int producer;
    int ticks;
    efd_craft_item *input;
    efd_craft_item *output;
} efd_craft;

typedef struct efd_tooltip {
    const char *title;
    const char *description;
} efd_tooltip;

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
    efd_result (*player_join_cb)(efd_entity player_id);
    efd_result (*player_leave_cb)(efd_entity player_id);
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
    efd_item *items;
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

    BLOCK_STONE = EFD_ID(EFD_TYPE_BLOCK),
    BLOCK_BRICK,
    BLOCK_HILL,
    BLOCK_HILL_SNOW,
    BLOCK_FENCE,
    BLOCK_WOOD,
    BLOCK_WALL,
    BLOCK_BELT_LEFT,
    BLOCK_BELT_RIGHT,
    BLOCK_BELT_UP,
    BLOCK_BELT_DOWN,

    ENTITY_PLAYER = EFD_ID(EFD_TYPE_ENTITY),
    ENTITY_MONSTER,
    ENTITY_TRUCK,
    ENTITY_TREE,
    ENTITY_CHEST,
    ENTITY_FURNACE,
    ENTITY_CRAFTBENCH,
    ENTITY_ASSEMBLER,

    ITEM_IRON_ORE = EFD_ID(EFD_TYPE_ITEM),
    ITEM_IRON_INGOT,
    ITEM_IRON_PLATE,
    ITEM_SCREW,
    ITEM_BELT,
};

// void Move(ecs_iter_t *it) {
//     // Get fields from system query
//     Position *p = ecs_field(it, Position, 1);
//     Velocity *v = ecs_field(it, Velocity, 2);

//     // Iterate matched entities
//     for (int i = 0; i < it->count, i++) {
//         p[i].x += v[i].x;
//         p[i].y += v[i].y;
//     }
// }

efd_result init() {
    // NOTE: control sets could be implemented later on
    // // register game specific input bindings
    // // pre-defined bindings and controlsets
    // efd_controlset_apply(EFD_ACTION_MOVE, EFD_CONTROLSET_WASD | EFD_CONTROLSET_ARROWS | EFD_CONTROLSET_GAMEPAD_LEFT);
    // efd_controlset_apply(EFD_ACTION_POINT, EFD_CONTROLSET_MOUSE | EFD_CONTROLSET_GAMEPAD_RIGHT);

    // // custom bindings and controlsets
    // efd_controlset_keyboard(CONTROLSET_ACCELERATE, EFD_KEY_DOWN, EFD_KEY_SHIFT_LEFT | EFD_KEY_SHIFT_RIGHT);
    // efd_controlset_gamepad(CONTROLSET_ACCELERATE, EFD_GAMEPAD_LEFT_TRIGGER, 0.5f);
    // efd_controlset_apply(EFD_ACTION_SPRINT, CONTROLSET_ACCERLATE);

    // // custom input bindings for custom actions
    // efd_controlset_register(ACTION_USE);
    // efd_controlset_keyboard(CONTROLSET_USE, EFD_KEY_DOWN, EFD_KEY_E);
    // efd_controlset_gamepad(CONTROLSET_USE, EFD_GAMEPAD_BUTTON_A, 0.5f);
    // efd_controlset_apply(ACTION_USE, CONTROLSET_USE);

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
    //     case EFD_ENTITY_PLAYER:
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

efd_result player_join(efd_entity player) {
    // efd_notify_push(player, "Test", "Welcome to the game!", 5.0f);
    return 0;
}

efd_result player_leave(efd_entity player) {
    return 0;
}

efd_result cleanup() {
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
            {EFD_TEXTURE, ENTITY_TREE, "assets/tree.png"},
            {EFD_TEXTURE, ENTITY_CHEST, "assets/chest.png"},
            {EFD_TEXTURE, ENTITY_FURNACE, "assets/furnace.png"},

            {EFD_TEXTURE, ENTITY_PLAYER, "assets/player.png"},
            {EFD_TEXTURE, ENTITY_MONSTER, "assets/monster.png"},

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
            {BLOCK_WALL, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_HILL, EFD_FLAG_COLLISION},
            {BLOCK_HILL_SNOW, EFD_FLAG_COLLISION},
            {BLOCK_FENCE, EFD_FLAG_COLLISION, .bounce = 1.0f},
            {BLOCK_WOOD, EFD_FLAG_COLLISION, .bounce = 0.0f},

            {BLOCK_BELT_LEFT, .velx = -150.0f},
            {BLOCK_BELT_RIGHT, .velx = 150.0f},
            {BLOCK_BELT_UP, .vely = -150.0f},
            {BLOCK_BELT_DOWN, .vely = 150.0f},

            {EFD_END},
        },

        .entities = (efd_entity[]){
            {ENTITY_TREE, EFD_FLAG_DESTROY_ON_COLLISION},
            {ENTITY_CHEST},
            {ENTITY_FURNACE},

            {ENTITY_PLAYER, EFD_FLAG_PLAYER},
            {ENTITY_TRUCK, EFD_FLAG_VEHICLE},
            {ENTITY_MONSTER, EFD_FLAG_AI},

            {EFD_END},
        },

        .items = (efd_item[]){
            // {ITEM_WOOD, "Wood", "A piece of wood."},
            // {ITEM_STONE, "Stone", "A piece of stone."},
            {EFD_END},
        },

        .crafting = (efd_craft[]){
            {
                .producer = ENTITY_FURNACE,
                .ticks = 20,
                .input = (efd_craft_item[]){
                    {ITEM_IRON_ORE, 1},
                    {EFD_END},
                },
                .output = (efd_craft_item[]){
                    {ITEM_IRON_PLATE, 4},
                    {EFD_END},
                },
            },
            {
                .producer = ENTITY_CRAFTBENCH,
                .ticks = 40,
                .input = (efd_craft_item[]){
                    {ITEM_IRON_PLATE, 1},
                    {EFD_END},
                },
                .output = (efd_craft_item[]){
                    {ITEM_SCREW, 8},
                    {EFD_END},
                },
            },
            {
                .producer = ENTITY_ASSEMBLER,
                .ticks = 120,
                .input = (efd_craft_item[]){
                    {BLOCK_FENCE, 1},
                    {ITEM_SCREW, 4},
                    {ITEM_IRON_PLATE, 2},
                    {EFD_END},
                },
                .output = (efd_craft_item[]){
                    {ITEM_BELT, 1},
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
    printf("ENTITY_PLAYER: %d\n", ENTITY_PLAYER);
    printf("ENTITY_CHEST: %d\n", ENTITY_CHEST);

    desc.init_cb();

    return 0;
}
