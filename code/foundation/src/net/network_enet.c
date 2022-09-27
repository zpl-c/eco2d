#include "zpl.h"

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(push, 0)
#include "enet.h"

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#define LIBRG_ENTITY_MAXCHUNKS 1
#include "librg.h"
#pragma warning(pop)

#include "net/network.h"
#include "pkt/packet.h"
#include "packets/pkt_00_init.h"
#include "world/world.h"
#include "core/game.h"
#include "ents/player.h"

#include "ecs/components.h"

#define NETWORK_UPDATE_DELAY 0.100

static ENetHost *host = NULL;
static ENetHost *server = NULL;
static ENetPeer *peer = NULL;
static librg_world *world = NULL;

int32_t network_init() {
    return enet_initialize() != 0;
}

int32_t network_destroy() {
    enet_deinitialize();
    return 0;
}

//~ NOTE(zaklaus): client

int32_t network_client_connect(const char *hostname, uint16_t port) {
    ENetAddress address = {0}; address.port = port;
    enet_address_set_host(&address, hostname);

    host = enet_host_create(NULL, 1, 2, 0, 0);
    peer = enet_host_connect(host, &address, 2, 0);

    if (peer == NULL) {
        zpl_printf("[ERROR] Cannot connect to specicied server: %s:%d\n", hostname, port);
        return 1;
    }

    world = librg_world_create();
    librg_world_userdata_set(world, peer);

    return 0;
}

int32_t network_client_disconnect() {
    enet_peer_disconnect(peer, 0);
    enet_host_destroy(host);

    librg_world_destroy(world);

    peer = NULL;
    host = NULL;
    world = NULL;

    return 0;
}

int32_t network_client_tick() {
    ENetEvent event = {0};

    while (enet_host_service(host, &event, 1) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                zpl_printf("[INFO] We connected to the server.\n");
                for (uint32_t i = 0; i < game_world_view_count(); i++) {
                    pkt_00_init_send(i);
                }
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                zpl_printf("[INFO] We disconnected from server.\n");
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                if (!world_read(event.packet->data, (uint32_t)event.packet->dataLength, event.peer)) {
                    zpl_printf("[INFO] Server sent us an unsupported packet.\n");
                }

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }

    return 0;
}

bool network_client_is_connected() {
    return peer ? enet_peer_get_state(peer) == ENET_PEER_STATE_CONNECTED : false;
}
network_client_stats
network_client_fetch_stats(void) {
    if (!network_client_is_connected())
        return (network_client_stats){0};

    network_client_stats stats = {0};

    stats.incoming_total = peer->incomingDataTotal;
    stats.total_received = peer->totalDataReceived;
    stats.outgoing_total = peer->outgoingDataTotal;
    stats.total_sent = peer->totalDataSent;

    static double next_measure = 0.0;
    static float incoming_bandwidth = 0.0f;
    static float outgoing_bandwidth = 0.0f;

    if (next_measure < get_cached_time()) {
#define MAX_RATE_SAMPLES 8
        static uint64_t last_total_sent = 0;
        static uint64_t last_total_recv = 0;
        static uint64_t rolling_counter = 0;
        static uint64_t sent_buffer[MAX_RATE_SAMPLES] = {0};
        static uint64_t recv_buffer[MAX_RATE_SAMPLES] = {0};

        uint64_t sent_delta = stats.total_sent - last_total_sent;
        uint64_t recv_delta = stats.total_received - last_total_recv;
        last_total_sent = stats.total_sent;
        last_total_recv = stats.total_received;

        sent_buffer[rolling_counter % MAX_RATE_SAMPLES] = sent_delta;
        recv_buffer[rolling_counter % MAX_RATE_SAMPLES] = recv_delta;
        ++rolling_counter;

        for (int i = 0; i < MAX_RATE_SAMPLES; i++) {
            stats.incoming_bandwidth += recv_buffer[i];
            stats.outgoing_bandwidth += sent_buffer[i];
        }

        incoming_bandwidth = stats.incoming_bandwidth /= MAX_RATE_SAMPLES;
        outgoing_bandwidth = stats.outgoing_bandwidth /= MAX_RATE_SAMPLES;

        next_measure = get_cached_time() + 1.0;
    } else {
        stats.incoming_bandwidth = incoming_bandwidth;
        stats.outgoing_bandwidth = outgoing_bandwidth;
    }

    stats.packets_sent = peer->totalPacketsSent;
    stats.packets_lost = peer->totalPacketsLost;

    if (stats.packets_sent > 0) {
        stats.packet_loss = stats.packets_lost / (float)stats.packets_sent;
    }

    stats.ping = peer->roundTripTime;
    stats.low_ping = peer->lowestRoundTripTime;

    return stats;
}

//~ NOTE(zaklaus): server

int32_t network_server_start(const char *host, uint16_t port) {
    (void)host;

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY;
    address.port = port;

    server = enet_host_create(&address, 8, 2, 0, 0);

    if (server == NULL) {
        zpl_printf("[ERROR] An error occured while trying to create a server host.\n");
        return 1;
    }

    return 0;
}

int32_t network_server_stop(void) {
    enet_host_destroy(server);
    server = 0;
    return 0;
}

int32_t network_server_tick(void) {
    ENetEvent event = {0};
    while (enet_host_service(server, &event, 1) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                zpl_printf("[INFO] A new user %d connected.\n", event.peer->incomingPeerID);
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                zpl_printf("[INFO] A user %d disconnected.\n", event.peer->incomingPeerID);
                network_server_despawn_viewers(event.peer);
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                if (!world_read(event.packet->data, (uint32_t)event.packet->dataLength, event.peer)) {
                    zpl_printf("[INFO] User %d sent us a malformed packet.\n", event.peer->incomingPeerID);
                }

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }

    return 0;
}

void network_server_despawn_viewers(void *peer_id) {
    ecs_iter_t it = ecs_query_iter(world_ecs(), world_ecs_clientinfo());

    while (ecs_query_next(&it)) {
        ClientInfo *p = ecs_field(&it, ClientInfo, 1);

        for (int i = 0; i < it.count; i++) {
            if (p[i].peer == (uintptr_t)peer_id) {
                player_despawn(it.entities[i]);
            }
        }
    }
}

uint64_t network_server_get_entity(void *peer_id, uint16_t view_id) {
    ecs_iter_t it = ecs_query_iter(world_ecs(), world_ecs_clientinfo());

    while (ecs_query_next(&it)) {
        ClientInfo *p = ecs_field(&it, ClientInfo, 1);

        for (int i = 0; i < it.count; i++) {
            if (p[i].peer == (uintptr_t)peer_id && p[i].view_id == view_id) {
                return it.entities[i];
            }
        }
    }

    return 0;
}

//~ NOTE(zaklaus): messaging

static int32_t network_msg_send_raw(ENetPeer *peer_id, void *data, size_t datalen, uint32_t flags, uint16_t channel_id) {
    if (peer_id == 0) peer_id = peer;
    ENetPacket *packet = enet_packet_create(data, datalen, flags);
    return enet_peer_send(peer_id, (enet_uint8)channel_id, packet);
}

int32_t network_msg_send(void *peer_id, void *data, size_t datalen, uint16_t channel_id) {
    return network_msg_send_raw(peer_id, data, datalen, ENET_PACKET_FLAG_RELIABLE, channel_id);
}

int32_t network_msg_send_unreliable(void *peer_id, void *data, size_t datalen, uint16_t channel_id) {
    return network_msg_send_raw(peer_id, data, datalen, 0, channel_id);
}
