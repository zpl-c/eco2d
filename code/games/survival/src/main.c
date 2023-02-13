#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/// lib.c

// core

typedef int efd_app;
typedef uint16_t efd_entity_type;
typedef int32_t efd_result;
typedef uint64_t efd_entity;

typedef enum efd_asset_type {
    // EFD_ASSET_NONE = 0,
    EFD_ASSET_TEXTURE,
    EFD_ASSET_ANIMATION,
    EFD_ASSET_SOUND,
    EFD_ASSET_FONT,
    EFD_ASSET_SHADER,
    EFD_ASSET_COUNT_TYPES,
} efd_asset_type;

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
} efd_app_desc;

// assets

// #define EFD_ASSET_TEXTURE_LAST 15
// #define EFD_ASSET_ANIMATION_LAST 2222222
// #define EFD_ASSET_ANIMATION_LAST 2222222

// #define CONC(a, b) a##b
/*  + CONC(type, _LAST) */

#define EFD_ASSET_SHIFT 16
#define EFD_ASSET(type) ((type) << EFD_ASSET_SHIFT)
#define EFD_ASSET_TYPE(id) ((id) >> EFD_ASSET_SHIFT)

typedef struct efd_asset {
    int id;
    const char *path;
    void *data; /* TODO: make a union */
} efd_asset;

efd_result efd_asset_add(int id, const char *path);
efd_result efd_asset_remove(int id);
efd_asset *efd_asset_get(int id);

// notifications

efd_result efd_notify_push(efd_entity actor, const char *title, const char *text, float duration);
efd_result efd_notify_clear(efd_entity actor);

// tooltips

efd_result efd_tooltip_add(const char *name, const char *text);
efd_result efd_tooltip_remove(const char *name);

// entities

enum efd_entity_type_builtins {
    EFD_ENTITY_NONE = 0,
    EFD_ENTITY_PLAYER = 1,
    EFD_ENTITY_CHUNK,
    EFD_ENTITY_OBJECT,
    EFD_ENTITY_ITEM,
    EFD_ENTITY_NPC,
    EFD_ENTITY_VEHICLE,

    EFD_ENTITY_TYPE_LAST,
    EFD_ENTITY_TYPE_MAX = 0xFFFF,
};

/// app.c

enum {
    /* textures */
    ASSET_TILE_DIRT = EFD_ASSET(EFD_ASSET_TEXTURE),
    ASSET_TILE_GRASS,
    ASSET_TILE_STONE,

    ASSET_BLOCK_STONE,
    ASSET_BLOCK_BRICK,

    ASSET_PLAYER,
    ASSET_TREE,
    ASSET_CHEST,
    ASSET_MONSTER,

    /* animations */
    ASSET_PLAYER_ANIM = EFD_ASSET(EFD_ASSET_ANIMATION),

    /* sounds */
    ASSET_PLAYER_SOUND = EFD_ASSET(EFD_ASSET_SOUND),
    ASSET_TREE_SOUND,
    ASSET_CHEST_SOUND,

    /* fonts */
    ASSET_FONT_DEFAULT = EFD_ASSET(EFD_ASSET_FONT),
};

enum {
    ENTITY_MONSTER = EFD_ENTITY_TYPE_LAST,
    ENTITY_WEAPON,
};

void Move(ecs_iter_t *it) {
    // Get fields from system query
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    // Iterate matched entities
    for (int i = 0; i < it->count, i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

efd_result init() {
    // register assets
    {
        // register textures (tiles)
        efd_asset_add(ASSET_TILE_DIRT, "assets/tile_dirt.png");
        efd_asset_add(ASSET_TILE_GRASS, "assets/tilegrass.png");
        efd_asset_add(ASSET_TILE_STONE, "assets/tilestone.png");

        // register textures (blocks)
        efd_asset_add(ASSET_BLOCK_STONE, "assets/block_stone.png");
        efd_asset_add(ASSET_BLOCK_BRICK, "assets/block_brick.png");

        // register textures (entities)
        efd_asset_add(ASSET_PLAYER, "assets/player.png");
        efd_asset_add(ASSET_TREE, "assets/tree.png");
        efd_asset_add(ASSET_CHEST, "assets/chest.png");
        efd_asset_add(ASSET_MONSTER, "assets/monster.png");

        // register animations
        efd_asset_add(ASSET_PLAYER_ANIM, "assets/player.anim");

        // register sounds
        efd_asset_add(ASSET_PLAYER_SOUND, "assets/player.ogg");
        efd_asset_add(ASSET_TREE_SOUND, "assets/tree.ogg");

        // register fonts
        efd_asset_add(ASSET_FONT_DEFAULT, "assets/font.ttf");
    }

    // define world generation rules, load existing world
    // or set up a custom pipeline to generate a new one
    // efd_world_load("worlds/default.json");

    // register tooltips
    efd_tooltip_add("ASSET_TREE", "A tree, it's a tree, what do you expect?");
    efd_tooltip_add("ASSET_CHEST", "A chest, it's a chest, what do you expect?");

    // 1. define itself
    // 2. render itself
    // 3. register types for sedning data
    // 4. define systems and components

    ECS_SYSTEM(efd_world(), Move, EcsOnUpdate, Position, [in] Velocity);

    // register game specific input bindings
    // pre-defined bindings and controlsets
    efd_controlset_apply(EFD_ACTION_MOVE, EFD_CONTROLSET_WASD | EFD_CONTROLSET_ARROWS | EFD_CONTROLSET_GAMEPAD_LEFT);
    efd_controlset_apply(EFD_ACTION_POINT, EFD_CONTROLSET_MOUSE | EFD_CONTROLSET_GAMEPAD_RIGHT);

    // custom bindings and controlsets
    efd_controlset_keyboard(CONTROLSET_ACCELERATE, EFD_KEY_DOWN, EFD_KEY_SHIFT_LEFT | EFD_KEY_SHIFT_RIGHT);
    efd_controlset_gamepad(CONTROLSET_ACCELERATE, EFD_GAMEPAD_LEFT_TRIGGER, 0.5f);
    efd_controlset_apply(EFD_ACTION_SPRINT, CONTROLSET_ACCERLATE);

    // custom input bindings for custom actions
    efd_controlset_register(ACTION_USE);
    efd_controlset_keyboard(CONTROLSET_USE, EFD_KEY_DOWN, EFD_KEY_E);
    efd_controlset_gamepad(CONTROLSET_USE, EFD_GAMEPAD_BUTTON_A, 0.5f);
    efd_controlset_apply(ACTION_USE, CONTROLSET_USE);

    return 0;
}

efd_result update() {
    return 0;
}

efd_result render(efd_entity_type type) {
    switch (type) {
        case EFD_ENTITY_PLAYER:
            /* additional/replacable rendering code on top of what EFD provides for built-in types */
            efd_render_texture(ASSET_PLAYER, 0, 0, 0, 0, 0, 0, 0, 0);

            return 1; /* we handled the rendering, don't render with the default renderer */
            break;

        case ENTITY_MONSTER:
            /* our custom rendering code for monster */
            efd_render_texture(ASSET_MONSTER, 0, 0, 0, 0, 0, 0, 0, 0);
            break;

        case ENTITY_WEAPON:
            /* our custom rendering code for weapon */
            break;
    }

    return 0;
}

efd_result player_join(efd_entity player) {
    efd_notify_push(player, "Test", "Welcome to the game!", 5.0f);
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
    };
}

/// test.c

#include <stdio.h>

efd_result efd_asset_add(int id, const char *path) {
    printf("asset_add: %d, %s\n", id, path);
    return 0;
}

efd_result efd_tooltip_add(const char *id, const char *text) {
    printf("tooltip_add: %s, %s\n", id, text);
    return 0;
}

efd_result efd_notify_push(efd_entity player, const char *title, const char *text, float duration) {
    printf("notify_push: %llu, %s, %s, %f\n", player, title, text, duration);
    return 0;
}

void test(efd_entity_type type) { printf("type: %d\n", type); }

int main(int argc, char **argv) {
    efd_app_desc desc = efd_main();
    printf("value: %f\n", desc.rules.item_pickup_radius);
    printf("ASSET_TILE_DIRT: %d\n", ASSET_TILE_DIRT);
    printf("ASSET_TILE_GRASS: %d\n", ASSET_TILE_GRASS);
    printf("ASSET_PLAYER: %d\n", ASSET_PLAYER);
    printf("ASSET_CHEST: %d\n", ASSET_CHEST);
    printf("ASSET_PLAYER_ANIM: %d\n", ASSET_PLAYER_ANIM);
    printf("ASSET_PLAYER_SOUND: %d\n", ASSET_PLAYER_SOUND);
    printf("ASSET_TREE_SOUND: %d\n", ASSET_TREE_SOUND);

    desc.init_cb();

    return 0;
}
