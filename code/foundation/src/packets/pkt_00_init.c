#include "packets/pkt_00_init.h"
#include "packets/pkt_01_welcome.h"
#include "pkt/packet.h"
#include "world/world.h"
#include "core/game.h"
#include "net/network.h"
#include "world/entity_view.h"
#include "core/camera.h"
#include "models/prefabs/player.h"
#include "models/entity.h"

#include "models/components.h"
#include "systems/systems.h"

pkt_desc pkt_00_init_desc[] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_00_init, view_id) },
    { PKT_END },
};

size_t pkt_00_init_send(uint16_t view_id) {
    pkt_00_init table = {.view_id = view_id };
    return pkt_world_write(MSG_ID_00_INIT, pkt_table_encode(pkt_00_init_desc, PKT_STRUCT_PTR(&table)), 1, view_id, NULL, 1);
}

int32_t pkt_00_init_handler(pkt_header *header) {
    pkt_00_init table;
    PKT_IF(pkt_msg_decode(header, pkt_00_init_desc, pkt_pack_desc_args(pkt_00_init_desc), PKT_STRUCT_PTR(&table)));

    uint64_t peer_id = (uint64_t)header->udata;
    uint64_t ent_id = player_spawn(NULL);

    entity_set_position(ent_id, world_dim()/2.0f + rand()%15*15.0f, world_dim()/2.0f + rand()%15*15.0f);

	game_player_joined(ent_id);

    zpl_printf("[INFO] initializing player entity id: %d with view id: %d for peer id: %d...\n", ent_id, table.view_id, peer_id);
    ecs_set(world_ecs(), ent_id, ClientInfo, {.peer = peer_id, .view_id = header->view_id, .active = false });
    pkt_01_welcome_send(world_seed(), peer_id, header->view_id, ent_id, world_chunk_size(), world_chunk_amount());
    return 0;
}
