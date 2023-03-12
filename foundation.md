## Foundation:
 * platform
 * viewer system ??
 * camera
 * game
 * debug ui
 * packet utils
 * arch
 * input
 * profiler
 * renderer
 * signal handling
 * zpl options
 * gen/textgen -> assets
 * items
 * inventory
 * crafting
 * notifications
 * tooltips
 * chunk
 * blocks
 * tiles (and chunk baker)
 * systems (core systems)
 * components
 * net
 * packets (but add custom messaging, and security)
 * compression
 * world
 * wrold_view
 * entity_view


## Components
-------
* app - thing that runs game
* game - the game stuff, includes client and server
* packet - structure that has data written/read by client/server
* asset - structure that describes tile/block/object/entity, something that can be visualized
* module - a thing that uses a set of ecs components and systems to create a self-contained ecs module
------------
* world - a map of chunks within the game world
* world-view - a representation of the world recreated by the client
----------
* chunk - entity that contains set of tiles and blocks
* tile - basic thing that makes up the chunks
* block - 2nd level of things that make up the chunk
* entity - an grid-independant static or dynamic entity that can exist in the world and has some systems controlling it
    primary archetypes:
    * item - an entity in the world, that can have a different state when its picked up
    * object - an item that can be placed in the world
    * device - an object that can be interacted with
    * mob - an entity that can be controlled by ai
    * player - an entity that can be controlled by a player
    * vehicle - an entity that can be used to transport other entities
* craft - a recipe that can be used to craft an item


## Naming

* zpl.eco
    * foundation
    * sandbox
    * survival
* prefix: efd_

## Concepts
    * tile
    * block
    * entity
        * item
        * object
        * device
        * mob
        * player
        * vehicle
    * craft

## Transitions
* item -> tile (item destroyed, tile replaced)
* item -> block (item destroyed, block created)
* item -> entity (item hidden and untracked, entity component added) // item becomes entity, just changes state
* item -> item (crafting) (item destroyed, item created)
* tile -> item (tile replaced, item created)
* block -> item (block destroyed, item created)
* block -> block (crafting) (block destroyed, block created)
* entity -> item (entity component removed, item shown and tracked) // entity becomes item, just changes state

## Features
* In-memory lists
    * assets
    * items
    * crafting recepies
    * prafabs for entities (pipeline for entities)
* built in server->client RPC
* 



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
