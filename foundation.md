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
* tile - basic thing that makes up the chunks
* block - 2nd level of things that make up the chunk
* chunk - entity that contains set of tiles and blocks
* item - an entity in the world, that can have a different state when its picked up
* crafting recipe - a recipe that can be used to craft an item
* entity - an grid-independant static or dynamic entity that can exist in the world and has some systems controlling it


## Naming

* zpl.eco
    * foundation
    * sandbox
    * survival
* prefix: efd_

## Objects
* entity
    * objects
    * players
    * npc
    * vehicles
    * items


## Features
* In-memory lists
    * assets
    * items
    * crafting recepies
    * prafabs for entities (pipeline for entities)
* built in server->client RPC
* 
