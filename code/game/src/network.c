#include "zpl.h"

#define ENET_IMPLEMENTATION
#include "enet.h"

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#define LIBRG_ENTITY_MAXCHUNKS 1
#include "librg.h"

#include "network.h"
#include "packet.h"
#include "packets/pkt_00_init.h"
#include "world/world.h"
#include "game.h"
#include "player.h"

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
    enet_peer_disconnect_now(peer, 0);
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
                pkt_00_init_send(0);
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                zpl_printf("[INFO] We disconnected from server.\n");
            } break;
            
            case ENET_EVENT_TYPE_RECEIVE: {
                if (!world_read(event.packet->data, event.packet->dataLength, event.peer)) {
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
                
                if (event.peer->data) {
                    player_despawn((ecs_entity_t)event.peer->data);
                    event.peer->data = 0;
                }
            } break;
            
            case ENET_EVENT_TYPE_RECEIVE: {
                if (!world_read(event.packet->data, event.packet->dataLength, event.peer)) {
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

void network_server_assign_entity(void *peer_id, uint64_t ent_id) {
    ENetPeer *peer = (ENetPeer *)peer_id;
    peer->data = (void*)ent_id;
}

uint64_t network_server_get_entity(void *peer_id) {
    if (game_get_kind() == GAMEKIND_SINGLE) {
        return (uint64_t)peer_id;
    }
    ENetPeer *peer = (ENetPeer *)peer_id;
    ZPL_ASSERT(peer->data);
    return (uint64_t)peer->data;
}

//~ NOTE(zaklaus): messaging

static int32_t network_msg_send_raw(ENetPeer *peer_id, void *data, size_t datalen, uint32_t flags, uint16_t channel_id) {
    if (peer_id == 0) peer_id = peer;
    ENetPacket *packet = enet_packet_create(data, datalen, flags);
    return enet_peer_send(peer_id, channel_id, packet);
}

int32_t network_msg_send(void *peer_id, void *data, size_t datalen, uint16_t channel_id) {
    return network_msg_send_raw(peer_id, data, datalen, ENET_PACKET_FLAG_RELIABLE, channel_id);
}

int32_t network_msg_send_unreliable(void *peer_id, void *data, size_t datalen, uint16_t channel_id) {
    return network_msg_send_raw(peer_id, data, datalen, 0, channel_id);
}
