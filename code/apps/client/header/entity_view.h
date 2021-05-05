#pragma once
#include "system.h"

#define ZPL_PICO
#include "zpl.h"

typedef struct entity_view {
    double X;
    double Y;
} entity_view;

ZPL_TABLE_DECLARE(, entity_view_tbl, entity_view_tbl_, entity_view);

void entity_view_init(void);
void entity_view_free(void);

void entity_view_update_or_create(uint64_t ent_id, entity_view data);
void entity_view_destroy(uint64_t ent_id);

entity_view *entity_view_get(uint64_t ent_id);
void entity_view_map(void (*map_proc)(uint64_t key, entity_view value));