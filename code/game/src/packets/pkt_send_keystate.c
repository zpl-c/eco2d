#include "packet_utils.h"
#include "packets/pkt_send_keystate.h"
#include "modules/components.h"
#include "modules/systems.h"
#include "world/world.h"

pkt_desc pkt_send_keystate_desc[] = {
    { PKT_REAL(pkt_send_keystate, x) },
    { PKT_REAL(pkt_send_keystate, y) },
    { PKT_UINT(pkt_send_keystate, use) },
    { PKT_UINT(pkt_send_keystate, sprint) },
    { PKT_END },
};

size_t pkt_send_keystate_send(uint16_t view_id,
                              float x,
                              float y,
                              uint8_t use,
                              uint8_t sprint) {
    pkt_send_keystate table = { .x = x, .y = y, .use = use, .sprint = sprint };
    return pkt_world_write(MSG_ID_SEND_KEYSTATE, pkt_send_keystate_encode(&table), 1, view_id, NULL);
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
    ecs_entity_t e = PKT_GET_ENT(header);
    
    Input *i = ecs_get_mut(world_ecs(), e, Input, NULL);
    if (i && !i->is_blocked) {
        i->x = table.x;
        i->y = table.y;
        i->use = table.use;
        i->sprint = table.sprint;
    }
    
    return 0;
}
