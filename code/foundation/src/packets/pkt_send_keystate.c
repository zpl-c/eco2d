#include "pkt/packet_utils.h"
#include "net/network.h"
#include "packets/pkt_send_keystate.h"
#include "models/components.h"
#include "systems/systems.h"
#include "world/world.h"
#include "models/entity.h"

#include "dev/debug_replay.h"

pkt_desc pkt_send_keystate_desc[] = {
    { PKT_REAL(pkt_send_keystate, x) },
    { PKT_REAL(pkt_send_keystate, y) },
    { PKT_REAL(pkt_send_keystate, mx) },
    { PKT_REAL(pkt_send_keystate, my) },
    { PKT_UINT(pkt_send_keystate, use) },
    { PKT_UINT(pkt_send_keystate, sprint) },
    { PKT_UINT(pkt_send_keystate, ctrl) },
    { PKT_UINT(pkt_send_keystate, pick) },
    { PKT_UINT(pkt_send_keystate, storage_action) },
    { PKT_UINT(pkt_send_keystate, selected_item) },
    { PKT_UINT(pkt_send_keystate, storage_selected_item) },
    { PKT_UINT(pkt_send_keystate, drop) },
    { PKT_UINT(pkt_send_keystate, swap) },
    { PKT_UINT(pkt_send_keystate, swap_storage) },
    { PKT_UINT(pkt_send_keystate, swap_from) },
    { PKT_UINT(pkt_send_keystate, swap_to) },
    { PKT_UINT(pkt_send_keystate, craft_item) },
    { PKT_UINT(pkt_send_keystate, placement_num) },
    { PKT_UINT(pkt_send_keystate, deletion_mode) },
    { PKT_ARRAY(pkt_send_keystate, placements) },
    { PKT_END },
};

pkt_desc pkt_send_blockpos_desc[] = {
    { PKT_REAL(pkt_send_blockpos, mx) },
    { PKT_REAL(pkt_send_blockpos, my) },
    { PKT_END },
};

size_t pkt_send_keystate_send(uint16_t view_id,
                              game_keystate_data *data) {
    return pkt_world_write(MSG_ID_SEND_KEYSTATE, pkt_table_encode(pkt_send_keystate_desc, PKT_STRUCT_PTR(data)), 1, view_id, NULL, 1);
}

size_t pkt_send_blockpos_send(uint16_t view_id,
                              pkt_send_blockpos *data){
    return pkt_world_write(MSG_ID_SEND_BLOCKPOS, pkt_table_encode(pkt_send_blockpos_desc, PKT_STRUCT_PTR(data)), 1, view_id, NULL, 1);
    
}

int32_t pkt_send_keystate_handler(pkt_header *header) {
    pkt_send_keystate table;
    PKT_IF(pkt_msg_decode(header, pkt_send_keystate_desc, pkt_pack_desc_args(pkt_send_keystate_desc), PKT_STRUCT_PTR(&table)));
    ecs_entity_t e = network_server_get_entity(header->udata, header->view_id);
    
    if (!world_entity_valid(e))
        return 1;
    
    Input *i = ecs_get_mut(world_ecs(), e, Input);
    if (i && !i->is_blocked) {
        i->x = zpl_clamp(table.x, -1.0f, 1.0f);
        i->y = zpl_clamp(table.y, -1.0f, 1.0f);
        i->mx = table.mx;
        i->my = table.my;
        i->use |= table.use;
        i->sprint = table.sprint;
        i->ctrl = table.ctrl;
        i->pick |= table.pick;
        i->selected_item = zpl_clamp(table.selected_item, 0, ITEMS_CONTAINER_SIZE-1);
        i->storage_selected_item = zpl_clamp(table.storage_selected_item, 0, ITEMS_CONTAINER_SIZE-1);
        i->drop |= table.drop;
        i->swap |= table.swap;
        i->swap_storage |= table.swap_storage;
        i->swap_from = zpl_clamp(table.swap_from, 0, ITEMS_CONTAINER_SIZE-1);
        i->swap_to = zpl_clamp(table.swap_to, 0, ITEMS_CONTAINER_SIZE-1);
        i->craft_item = table.craft_item;
        i->storage_action = table.storage_action;
        i->deletion_mode = table.deletion_mode;
        if (table.placement_num > 0) {
            i->num_placements = zpl_clamp(table.placement_num, 0, BUILD_MAX_PLACEMENTS);
            for (uint8_t j = 0; j < i->num_placements; j++) {
                i->placements_x[j] = table.placements[j].x;
                i->placements_y[j] = table.placements[j].y;
            }
        }
        debug_replay_record_keystate(table);
        entity_wake(e);
    }
    
    return 0;
}

int32_t pkt_send_blockpos_handler(pkt_header *header) {
    pkt_send_blockpos table;
    PKT_IF(pkt_msg_decode(header, pkt_send_blockpos_desc, pkt_pack_desc_args(pkt_send_blockpos_desc), PKT_STRUCT_PTR(&table)));
    ecs_entity_t e = network_server_get_entity(header->udata, header->view_id);
    
    if (!world_entity_valid(e))
        return 1;
    
    Input *i = ecs_get_mut(world_ecs(), e, Input);
    if (i && !i->is_blocked) {
        i->bx = table.mx;
        i->by = table.my;
    }
    
    return 0;
}
