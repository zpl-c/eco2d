#include "packets/pkt_send_notif.h"
#include "pkt/packet.h"
#include "world/world.h"
#include "core/game.h"
#include "world/entity_view.h"

#include "models/components.h"
#include "systems/systems.h"

// client
#include "gui/notifications.h"

pkt_desc pkt_send_notification_desc[] = {
	{ PKT_ARRAY(pkt_send_notification, title) },
	{ PKT_ARRAY(pkt_send_notification, text) },
	{ PKT_END },
};

size_t pkt_notification_send(uint64_t peer_id, uint16_t view_id, const char *title, const char *text) {
	pkt_send_notification table = { 0 };
	zpl_strncpy(table.title, title, sizeof(table.title));
	zpl_strncpy(table.text, text, sizeof(table.text));
	return pkt_world_write(MSG_ID_SEND_NOTIFICATION, pkt_table_encode(pkt_send_notification_desc, PKT_STRUCT_PTR(&table)), 1, view_id, (void*)peer_id, 0);
}

int32_t pkt_send_notification_handler(pkt_header *header) {
	pkt_send_notification table;
	PKT_IF(pkt_msg_decode(header, pkt_send_notification_desc, pkt_pack_desc_args(pkt_send_notification_desc), PKT_STRUCT_PTR(&table)));

	notification_push(table.title, table.text);

	return 0;
}
