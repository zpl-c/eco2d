#include "packets/pkt_01_welcome.h"
#include "packet.h"
#include "world/world.h"
#include "game.h"
#include "entity_view.h"
#include "camera.h"

pkt_desc pkt_01_welcome_desc[] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, seed) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, ent_id) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, chunk_size) },
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_01_welcome, world_size) },
    { PKT_END },
};

size_t pkt_01_welcome_send(uint32_t seed,
                           uint64_t peer_id,
                           uint16_t view_id,
                           uint64_t ent_id,
                           uint16_t chunk_size,
                           uint16_t world_size) {
    pkt_01_welcome table = {.seed = seed, .ent_id = ent_id, .chunk_size = chunk_size, .world_size = world_size};
    return pkt_world_write(MSG_ID_01_WELCOME, pkt_table_encode(pkt_01_welcome_desc, PKT_STRUCT_PTR(&table)), 1, view_id, (void*)peer_id, 0);
}

int32_t pkt_01_welcome_handler(pkt_header *header) {
    pkt_01_welcome table;
    PKT_IF(pkt_msg_decode(header, pkt_01_welcome_desc, pkt_pack_desc_args(pkt_01_welcome_desc), PKT_STRUCT_PTR(&table)));

    world_view *view = game_world_view_get(header->view_id);

    zpl_printf("[INFO] initializing read-only world view id: %d...\n", header->view_id);
    world_view_init(view, table.seed, table.ent_id, table.chunk_size, table.world_size);
    game_world_view_set_active(view);
    return 0;
}
