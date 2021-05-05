#include "entity_view.h"

ZPL_TABLE_DEFINE(entity_view_tbl, entity_view_tbl_, entity_view);

static entity_view_tbl cli_entities = {0};


void entity_view_init(void) {
    entity_view_tbl_init(&cli_entities, zpl_heap());
}

void entity_view_free(void) {
    entity_view_tbl_destroy(&cli_entities);
}

void entity_view_update_or_create(uint64_t ent_id, entity_view data) {
    entity_view_tbl_set(&cli_entities, ent_id, data);
}

void entity_view_destroy(uint64_t ent_id) {
    entity_view_tbl_remove(&cli_entities, ent_id);
}

entity_view *entity_view_get(uint64_t ent_id) {
    return entity_view_tbl_get(&cli_entities, ent_id);
}

void entity_view_map(void (*map_proc)(uint64_t key, entity_view value)) {
    entity_view_tbl_map(&cli_entities, map_proc);
}
