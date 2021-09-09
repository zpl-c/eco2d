#include "packet_utils.h"
#include "network.h"
#include "packets/pkt_send_keystate.h"
#include "modules/components.h"
#include "modules/systems.h"
#include "world/world.h"

#include "debug_replay.h"

pkt_desc pkt_send_keystate_desc[] = {
    { PKT_REAL(pkt_send_keystate, x) },
    { PKT_REAL(pkt_send_keystate, y) },
    { PKT_REAL(pkt_send_keystate, mx) },
    { PKT_REAL(pkt_send_keystate, my) },
    { PKT_UINT(pkt_send_keystate, use) },
    { PKT_UINT(pkt_send_keystate, sprint) },
    { PKT_UINT(pkt_send_keystate, ctrl) },
    { PKT_UINT(pkt_send_keystate, selected_item) },
    { PKT_UINT(pkt_send_keystate, drop) },
    { PKT_UINT(pkt_send_keystate, swap) },
    { PKT_UINT(pkt_send_keystate, swap_from) },
    { PKT_UINT(pkt_send_keystate, swap_to) },
    { PKT_END },
};

size_t pkt_send_keystate_send(uint16_t view_id,
                              float x,
                              float y,
                              float mx,
                              float my,
                              uint8_t use,
                              uint8_t sprint,
                              uint8_t ctrl,
                              uint8_t drop,
                              uint8_t selected_item,
                              uint8_t swap,
                              uint8_t swap_from,
                              uint8_t swap_to) {
    pkt_send_keystate table = { .x = x, .y = y, .mx = mx, .my = my, .use = use, .sprint = sprint, .ctrl = ctrl, .drop = drop, .selected_item = selected_item, .swap = swap, .swap_from = swap_from, .swap_to = swap_to };
    return pkt_world_write(MSG_ID_SEND_KEYSTATE, pkt_send_keystate_encode(&table), 1, view_id, NULL, 1);
}

size_t pkt_send_keystate_encode(pkt_send_keystate *table) {
    cw_pack_context pc = {0};
    pkt_pack_msg(&pc, pkt_pack_desc_args(pkt_send_keystate_desc));
    pkt_pack_struct(&pc, pkt_send_keystate_desc, PKT_STRUCT_PTR(table));
    return pkt_pack_msg_size(&pc);
}

int32_t pkt_send_keystate_handler(pkt_header *header) {
    pkt_send_keystate table;
    PKT_IF(pkt_msg_decode(header, pkt_send_keystate_desc, pkt_pack_desc_args(pkt_send_keystate_desc), PKT_STRUCT_PTR(&table)));
    ecs_entity_t e = network_server_get_entity(header->udata);
    
    if (!world_entity_valid(e))
        return 1;
    
    Input *i = ecs_get_mut(world_ecs(), e, Input, NULL);
    if (i && !i->is_blocked) {
        i->x = zpl_clamp(table.x, -1.0f, 1.0f);
        i->y = zpl_clamp(table.y, -1.0f, 1.0f);
        i->mx = table.mx;
        i->my = table.my;
        i->use = table.use;
        i->sprint = table.sprint;
        i->ctrl = table.ctrl;
        i->selected_item = zpl_clamp(table.selected_item, 0, ITEMS_INVENTORY_SIZE-1);
        i->drop = table.drop;
        i->swap = table.swap;
        i->swap_from = zpl_clamp(table.swap_from, 0, ITEMS_INVENTORY_SIZE-1);
        i->swap_to = zpl_clamp(table.swap_to, 0, ITEMS_INVENTORY_SIZE-1);
        debug_replay_record_keystate(table);
    }
    
    return 0;
}
