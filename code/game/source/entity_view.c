#include "entity_view.h"
#include "packet_utils.h"
#include "world/blocks.h"

ZPL_TABLE_DEFINE(entity_view_tbl, entity_view_tbl_, entity_view);

// TODO(zaklaus): Fix conditional streaming
pkt_desc pkt_entity_view_desc[] = {
    { PKT_UINT(entity_view, kind) },
    { PKT_UINT(entity_view, flag) },
    { PKT_HALF(entity_view, x) },
    { PKT_HALF(entity_view, y) },
    //{ PKT_SKIP_IF(entity_view, blocks_used, 1, 2) }, // NOTE(zaklaus): skip velocity for chunks
    { PKT_HALF(entity_view, vx) },
    { PKT_HALF(entity_view, vy) },
    //{ PKT_SKIP_IF(entity_view, blocks_used, 0, 1) }, // NOTE(zaklaus): skip blocks for anything else
    { PKT_ARRAY(entity_view, blocks) },
    { PKT_HALF(entity_view, hp) },
    { PKT_HALF(entity_view, max_hp) },
    { PKT_END }, 
};

size_t entity_view_pack_struct(void *data, size_t len, entity_view view) {
    cw_pack_context pc = {0};
    cw_pack_context_init(&pc, data, (unsigned long)len, 0);
    pkt_pack_struct(&pc, pkt_entity_view_desc, PKT_STRUCT_PTR(&view));
    return pc.current - pc.start;
}

entity_view entity_view_unpack_struct(void *data, size_t len) {
    cw_unpack_context uc = {0};
    cw_unpack_context_init(&uc, data, (unsigned long)len, 0);
    
    entity_view view = {0};
    pkt_unpack_struct(&uc, pkt_entity_view_desc, PKT_STRUCT_PTR(&view));
#if 0
    if (view.kind == EKIND_CHUNK) {
        for (int i=0;i<256;i++){
            zpl_printf("%d, ", view.blocks[i]);
        }
        zpl_exit(0);
    }
#endif
    
    return view;
}

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
    entity_view_remove_chunk_texture(map, ent_id);
    entity_view_tbl_remove(map, ent_id);
}

entity_view *entity_view_get(entity_view_tbl *map, uint64_t ent_id) {
    return entity_view_tbl_get(map, ent_id);
}

void entity_view_map(entity_view_tbl *map, void (*map_proc)(uint64_t key, entity_view * value)) {
    entity_view_tbl_map_mut(map, map_proc);
}

void entity_view_mark_for_removal(entity_view_tbl *map, uint64_t ent_id) {
    entity_view *view = entity_view_tbl_get(map, ent_id);
    view->tran_effect = ETRAN_FADEOUT;
    view->tran_time = 1.0f;
}

void entity_view_mark_for_fadein(entity_view_tbl *map, uint64_t ent_id) {
    entity_view *view = entity_view_tbl_get(map, ent_id);
    view->tran_effect = ETRAN_FADEIN;
    view->tran_time = 0.0f;
}

void entity_view_update_chunk_texture(entity_view_tbl *map, uint64_t ent_id, void *world_view) {
    entity_view *view = entity_view_tbl_get(map, ent_id);
    if (view->kind != EKIND_CHUNK) return;
    blocks_build_chunk_tex(ent_id, view->blocks, sizeof(view->blocks), world_view);
}

void entity_view_remove_chunk_texture(entity_view_tbl *map, uint64_t ent_id) {
    entity_view *view = entity_view_tbl_get(map, ent_id);
    if (view->kind != EKIND_CHUNK) return;
    blocks_remove_chunk_tex(view->tex);
}