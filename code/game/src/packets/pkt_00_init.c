#include "packets/pkt_00_init.h"
#include "packets/pkt_01_welcome.h"
#include "packet.h"
#include "world/world.h"
#include "game.h"
#include "entity_view.h"
#include "camera.h"
#include "player.h"

#include "modules/components.h"
#include "modules/systems.h"

pkt_desc pkt_00_init_desc[] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_00_init, view_id) },
    { PKT_END },
};

size_t pkt_00_init_encode(pkt_00_init *table) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, pkt_pack_desc_args(pkt_00_init_desc));
    pkt_pack_struct(&pc, pkt_00_init_desc, PKT_STRUCT_PTR(table));
    return pkt_pack_msg_size(&pc);
}

size_t pkt_00_init_send(uint16_t view_id) {
    pkt_00_init table = {.view_id = view_id };
    return pkt_world_write(MSG_ID_00_INIT, pkt_00_init_encode(&table), 1, view_id, NULL);
}

int32_t pkt_00_init_handler(pkt_header *header) {
    pkt_00_init table;
    PKT_IF(pkt_msg_decode(header, pkt_00_init_desc, pkt_pack_desc_args(pkt_00_init_desc), PKT_STRUCT_PTR(&table)));
    
    uint64_t peer_id = (uint64_t)header->udata;
    uint64_t ent_id = player_spawn(NULL);
    ecs_set(world_ecs(), ent_id, ClientInfo, {.peer = ent_id, .view_id = header->view_id });
    pkt_01_welcome_send(world_seed(), peer_id, header->view_id, ent_id, world_chunk_size(), world_chunk_amount());
    return 0;
}
