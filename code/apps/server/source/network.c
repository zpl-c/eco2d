#include "zpl.h"

#define ENET_IMPLEMENTATION
#include "enet.h"

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#include "librg.h"

#include "system.h"
#include "network.h"
#include "packet.h"
#include "world/world.h"

#include "player.h"

#include "modules/general.h"
#include "modules/controllers.h"
#include "modules/net.h"

#include "assets.h"
#include "packets/pkt_01_welcome.h"

#define NETWORK_UPDATE_DELAY 0.100
#define NETWORK_MAX_CLIENTS 32

static ENetHost *server = NULL;

WORLD_PKT_WRITER(mp_pkt_writer) {
    if (pkt->is_reliable) {
        return network_msg_send(udata, pkt->data, pkt->datalen);
    }
    else {
        return network_msg_send_unreliable(udata, pkt->data, pkt->datalen);
    }
}

int32_t network_init(void) {
    return enet_initialize() != 0;
}

int32_t network_destroy(void) {
    enet_deinitialize();
    return 0;
}

int32_t network_server_start(const char *host, uint16_t port) {
    zpl_unused(host);

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost. */
    address.port = port; /* Bind the server to port. */

    /* create a server */
    server = enet_host_create(&address, NETWORK_MAX_CLIENTS, 2, 0, 0);

    if (server == NULL) {
        zpl_printf("[ERROR] An error occurred while trying to create an ENet server host.\n");
        return 1;
    }

    zpl_printf("[INFO] Started an ENet server...\n");

    return 0;
}

int32_t network_server_stop(void) {
    zpl_printf("[INFO] Shutting down the ENet server...\n");
    enet_host_destroy(server);
    server = NULL;
    return 0;
}

int32_t network_server_tick(void) {
    ENetEvent event = {0};
    while (enet_host_service(server, &event, 1) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                zpl_printf("[INFO] A new user %d connected.\n", event.peer->incomingPeerID);
                uint16_t peer_id = event.peer->incomingPeerID;
                uint64_t ent_id = network_client_create(event.peer);
                // TODO: Make sure ent_id does not get truncated with large entity numbers.
                event.peer->data = (void*)((uint32_t)ent_id);
                
                pkt_01_welcome table = {.ent_id = ent_id, .block_size = world_block_size(), .chunk_size = world_chunk_size(), .world_size = world_world_size()};
                pkt_world_write(MSG_ID_01_WELCOME, pkt_01_welcome_encode(&table), 1, event.peer);
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                zpl_printf("[INFO] A user %d disconnected.\n", event.peer->incomingPeerID);
                ecs_entity_t e = (ecs_entity_t)((uint32_t)event.peer->data);
                network_client_destroy(e);
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                if (!world_read(event.packet->data, event.packet->dataLength, event.peer)) {
                    zpl_printf("[INFO] User %d sent us a malformed packet.\n", event.peer->incomingPeerID);
                    ecs_entity_t e = (ecs_entity_t)((uint32_t)event.peer->data);
                    network_client_destroy(e);
                }

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }

    return 0;
}

uint64_t network_client_create(ENetPeer *peer) {
    ECS_IMPORT(world_ecs(), Net);
    ecs_entity_t e = (ecs_entity_t)player_spawn(zpl_bprintf("client_%d", peer->incomingPeerID));
    ecs_set(world_ecs(), e, ClientInfo, {(uintptr_t)peer});

    librg_entity_owner_set(world_tracker(), e, (int64_t)peer);

    return (uint64_t)e;
}

void network_client_destroy(uint64_t ent_id) {
    player_despawn(ent_id);
}

static int32_t network_msg_send_raw(ENetPeer *peer, void *data, size_t datalen, uint32_t flags) {
    ENetPacket *packet = enet_packet_create(data, datalen, flags);
    return enet_peer_send(peer, 0, packet);
}

int32_t network_msg_send(ENetPeer *peer, void *data, size_t datalen) {
    return network_msg_send_raw(peer, data, datalen, ENET_PACKET_FLAG_RELIABLE);
}

int32_t network_msg_send_unreliable(ENetPeer *peer, void *data, size_t datalen) {
    return network_msg_send_raw(peer, data, datalen, 0);
}
