#pragma once
#include "system.h"

#define ZPL_PICO
#include "zpl.h"

#include "packet_utils.h"

typedef enum {
    EKIND_PLAYER,
    EKIND_THING,
    EKIND_CHUNK,
    FORCE_EKIND_UINT16 = UINT16_MAX
} entity_kind;

typedef struct entity_view {
    entity_kind kind;
    float x;
    float y;
} entity_view;

ZPL_TABLE_DECLARE(, entity_view_tbl, entity_view_tbl_, entity_view);

pkt_desc pkt_entity_view_desc[];

void entity_view_init(entity_view_tbl *map);
void entity_view_free(entity_view_tbl *map);

void entity_view_update_or_create(entity_view_tbl *map, uint64_t ent_id, entity_view data);
void entity_view_destroy(entity_view_tbl *map, uint64_t ent_id);

entity_view *entity_view_get(entity_view_tbl *map, uint64_t ent_id);
void entity_view_map(entity_view_tbl *map, void (*map_proc)(uint64_t key, entity_view value));

size_t entity_view_pack_struct(void *data, size_t len, entity_view view);
entity_view entity_view_unpack_struct(void *data, size_t len);
