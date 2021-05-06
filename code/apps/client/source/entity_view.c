#include "entity_view.h"

ZPL_TABLE_DEFINE(entity_view_tbl, entity_view_tbl_, entity_view);

void entity_view_init(entity_view_tbl *map) {
    entity_view_tbl_init(map, zpl_heap());
}

void entity_view_free(entity_view_tbl *map) {
    entity_view_tbl_destroy(map);
}

void entity_view_update_or_create(entity_view_tbl *map, uint64_t ent_id, entity_view data) {
    entity_view_tbl_set(map, ent_id, data);
}

void entity_view_destroy(entity_view_tbl *map, uint64_t ent_id) {
    entity_view_tbl_remove(map, ent_id);
}

entity_view *entity_view_get(entity_view_tbl *map, uint64_t ent_id) {
    return entity_view_tbl_get(map, ent_id);
}

void entity_view_map(entity_view_tbl *map, void (*map_proc)(uint64_t key, entity_view value)) {
    entity_view_tbl_map(map, map_proc);
}
