#include "packets/pkt_send_code.h"
#include "pkt/packet.h"
#include "world/world.h"
#include "core/game.h"
#include "world/entity_view.h"

#include "models/components.h"
#include "systems/systems.h"

pkt_desc pkt_send_code_desc[] = {
	{ PKT_UINT(pkt_send_code, code) },
	{ PKT_ARRAY(pkt_send_code, params) },
	{ PKT_ARRAY(pkt_send_code, data) },
	{ PKT_END },
};

size_t pkt_code_send(uint64_t peer_id, uint16_t view_id, pkt_send_code table) {
	return pkt_world_write(MSG_ID_SEND_CODE, pkt_table_encode(pkt_send_code_desc, PKT_STRUCT_PTR(&table)), 1, view_id, (void*)peer_id, 0);
}

int32_t pkt_send_code_handler(pkt_header *header) {
	pkt_send_code table = { 0 };
	PKT_IF(pkt_msg_decode(header, pkt_send_code_desc, pkt_pack_desc_args(pkt_send_code_desc), PKT_STRUCT_PTR(&table)));

	game_client_receive_code(table);

	return 0;
}
