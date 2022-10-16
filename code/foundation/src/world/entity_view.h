#pragma once
#include "platform/system.h"
#include "models/assets.h"
#include "models/items.h"

#define ZPL_PICO
#include "zpl.h"

typedef enum {
    EKIND_SERVER = 0,
    EKIND_PLAYER,
    EKIND_ITEM,
    EKIND_DEVICE,
    EKIND_VEHICLE,
    EKIND_DEMO_NPC,
    EKIND_MONSTER,
    EKIND_MACRO_BOT,
    EKIND_CHUNK,
    FORCE_EKIND_UINT16 = UINT16_MAX
} entity_kind;

typedef enum {
    EFLAG_INTERP = (1 << 0),
    FORCE_EFLAG_UINT16 = UINT16_MAX
} entity_flag;

typedef enum {
    ETRAN_NONE,
    ETRAN_FADEOUT,
    ETRAN_FADEIN,
    ETRAN_REMOVE,
    FORCE_ETRAN_UINT8 = UINT8_MAX
} entity_transition_effect;

typedef struct entity_view {
    int64_t ent_id;
    entity_kind kind;
    entity_flag flag;
    float x;
    float y;
    float vx;
    float vy;
    float tx;
    float ty;

    float hp;
    float max_hp;

    // TODO(zaklaus): Find a way to stream dynamic arrays
    uint32_t chk_id;
    uint8_t blocks_used;
    block_id blocks[256];
    block_id outer_blocks[256];
    uint32_t color;
    uint8_t is_dirty;
    int64_t tex;

    // NOTE(zaklaus): vehicle
    float heading, theading;
    bool inside_vehicle;
    uint32_t veh_kind;

    // NOTE(zaklaus): items, ...
    asset_id asset;
    uint32_t quantity;
    float durability;

    // NOTE(zaklaus): device progress bar
    uint32_t progress_active;
    float progress_value;

    // NOTE(zaklaus): inventory
    uint8_t has_items;
    Item items[ITEMS_INVENTORY_SIZE];
    uint8_t selected_item;

    // NOTE(zaklaus): storage interface
    uint8_t has_storage_items;
    Item storage_items[ITEMS_CONTAINER_SIZE];
    uint8_t storage_selected_item;

    // NOTE(zaklaus): entity picking
    uint64_t pick_ent;
    uint64_t sel_ent;

    // NOTE(zaklaus): internals
    uint8_t layer_id;
    uint64_t last_update;

    // NOTE(zaklaus): fade in-out effect
    entity_transition_effect tran_effect;
    float tran_time;
} entity_view;

ZPL_TABLE_DECLARE(, entity_view_tbl, entity_view_tbl_, entity_view);

void entity_view_init(entity_view_tbl *map);
void entity_view_free(entity_view_tbl *map);

void entity_view_update_or_create(entity_view_tbl *map, uint64_t ent_id, entity_view data);
void entity_view_destroy(entity_view_tbl *map, uint64_t ent_id);

entity_view *entity_view_get(entity_view_tbl *map, uint64_t ent_id);
void entity_view_map(entity_view_tbl *map, void (*map_proc)(uint64_t key, entity_view *value));

size_t entity_view_pack_struct(void *data, size_t len, entity_view *view);
entity_view entity_view_unpack_struct(void *data, size_t len);

void entity_view_mark_for_removal(entity_view_tbl *map, uint64_t ent_id);
void entity_view_mark_for_fadein(entity_view_tbl *map, uint64_t ent_id);
void entity_view_update_chunk_texture(entity_view_tbl *map, uint64_t ent_id, void *view);
void entity_view_remove_chunk_texture(entity_view_tbl *map, uint64_t ent_id);
