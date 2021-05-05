#include "packets/pkt_01_welcome.h"
#include "packet.h"
#include "world/world.h"
#include "game.h"

#ifdef CLIENT
#include "entity_view.h"
#include "camera.h"
#endif

pkt_desc pkt_01_welcome_desc[] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, ent_id) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, block_size) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, chunk_size) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, world_size) },
    { PKT_END },
};

size_t pkt_01_welcome_encode(pkt_01_welcome *table) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, pkt_pack_desc_args(pkt_01_welcome_desc));
    pkt_pack_struct(&pc, pkt_01_welcome_desc, PKT_STRUCT_PTR(table));
    return pkt_pack_msg_size(&pc);
}

int32_t pkt_01_welcome_handler(pkt_header *header) {
    pkt_01_welcome table;
    PKT_IF(pkt_msg_decode(header, pkt_01_welcome_desc, pkt_pack_desc_args(pkt_01_welcome_desc), PKT_STRUCT_PTR(&table)));

    #ifdef CLIENT
    if (game_is_networked()) {
        zpl_printf("[INFO] initializing read-only world view ...\n");
        world_init_minimal(table.block_size, table.chunk_size, table.world_size, NULL, NULL);
    }
    
    entity_view_update_or_create(table.ent_id, (entity_view){0});
    camera_set_follow(table.ent_id);
    #endif
    return 0;
}
