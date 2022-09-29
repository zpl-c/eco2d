#include "packets/pkt_switch_viewer.h"
#include "pkt/packet.h"
#include "world/world.h"
#include "core/game.h"
#include "net/network.h"
#include "world/entity_view.h"
#include "core/camera.h"
#include "models/prefabs/player.h"

#include "models/components.h"
#include "systems/systems.h"

pkt_desc pkt_switch_viewer_desc[] = {
    { PKT_FIELD(CWP_ITEM_POSITIVE_INTEGER, pkt_switch_viewer, view_id) },
    { PKT_END },
};

size_t pkt_switch_viewer_send(uint16_t view_id) {
    pkt_switch_viewer table = {.view_id = view_id };
    return pkt_world_write(MSG_ID_SWITCH_VIEWER, pkt_table_encode(pkt_switch_viewer_desc, PKT_STRUCT_PTR(&table)), 1, view_id, NULL, 1);
}

int32_t pkt_switch_viewer_handler(pkt_header *header) {
    pkt_switch_viewer table;
    PKT_IF(pkt_msg_decode(header, pkt_switch_viewer_desc, pkt_pack_desc_args(pkt_switch_viewer_desc), PKT_STRUCT_PTR(&table)));

    ecs_entity_t e = network_server_get_entity(header->udata, header->view_id);
    uint64_t peer_id = (uint64_t)header->udata;

    if (!world_entity_valid(e))
        return 1;

    ecs_iter_t it = ecs_query_iter(world_ecs(), world_ecs_clientinfo());

    while (ecs_query_next(&it)) {
        ClientInfo *p = ecs_field(&it, ClientInfo, 1);

        for (int i = 0; i < it.count; i++) {
            if (p[i].peer == (uintptr_t)peer_id && p[i].view_id == table.view_id) {
                p[i].active = true;
            } else if (p[i].peer == (uintptr_t)peer_id) {
                p[i].active = false;
            }
        }
    }

    return 0;
}
