#include "zpl.h"

#define ENET_IMPLEMENTATION
#include "enet.h"

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#define LIBRG_ENTITY_MAXCHUNKS 1
#include "librg.h"

#include "network.h"
#include "packet.h"
#include "world/world.h"

#define NETWORK_UPDATE_DELAY 0.100

static ENetHost *host = NULL;
static ENetPeer *peer = NULL;
static librg_world *world = NULL;

int32_t network_init() {
    return enet_initialize() != 0;
}

int32_t network_destroy() {
    enet_deinitialize();
    return 0;

}


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

#if 0
    librg_event_set(world, LIBRG_READ_CREATE, client_read_create);
    librg_event_set(world, LIBRG_READ_UPDATE, client_read_update);
    librg_event_set(world, LIBRG_READ_REMOVE, client_read_remove);
#endif

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
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                zpl_printf("[INFO] We disconnected from server.\n");
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                if (!world_read(event.packet->data, event.packet->dataLength, NULL)) {
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

static int32_t network_msg_send_raw(uint16_t peer_id, void *data, size_t datalen, uint32_t flags) {
    ENetPacket *packet = enet_packet_create(data, datalen, flags);
    return enet_peer_send(peer, 0, packet);
}

int32_t network_msg_send(void *data, size_t datalen) {
    return network_msg_send_raw(0, data, datalen, ENET_PACKET_FLAG_RELIABLE);
}

int32_t network_msg_send_unreliable(void *data, size_t datalen) {
    return network_msg_send_raw(0, data, datalen, 0);
}
