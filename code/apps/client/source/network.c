#include "zpl.h"

#define ENET_IMPLEMENTATION
#include "enet.h"

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#include "librg.h"

#include "network.h"

#define NETWORK_UPDATE_DELAY 0.100

static ENetHost *host = NULL;
static ENetPeer *peer = NULL;
static librg_world *world = NULL;
static zpl_timer nettimer = {0};


int32_t network_init() {
    zpl_timer_set(&nettimer, NETWORK_UPDATE_DELAY, -1, network_client_update);
    return enet_initialize() != 0;
}

int32_t network_destroy() {
    enet_deinitialize();
    return 0;

}

int32_t client_read_create(librg_world *w, librg_event *e) {
    int64_t owner_id = librg_event_owner_get(w, e);
    int64_t entity_id = librg_event_entity_get(w, e);
    zpl_printf("[INFO] An entity %d was created for owner: %d\n", (int)entity_id, (int)owner_id);
    return 0;
}

int32_t client_read_remove(librg_world *w, librg_event *e) {
    int64_t owner_id = librg_event_owner_get(w, e);
    int64_t entity_id = librg_event_entity_get(w, e);
    zpl_printf("[INFO] An entity %d was removed for owner: %d\n", (int)entity_id, (int)owner_id);
    return 0;
}

int32_t client_read_update(librg_world *w, librg_event *e) {
    // int64_t entity_id = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    // if (actual_length != sizeof(vec3)) return 0;

    // vec3 position = {0};
    // memcpy(&position, buffer, actual_length);

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

    librg_event_set(world, LIBRG_READ_CREATE, client_read_create);
    librg_event_set(world, LIBRG_READ_UPDATE, client_read_update);
    librg_event_set(world, LIBRG_READ_REMOVE, client_read_remove);

    zpl_timer_start(&nettimer, NETWORK_UPDATE_DELAY);

    return 0;
}

int32_t network_client_disconnect() {
    zpl_timer_stop(&nettimer);

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
    #define ID 0

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
                /* handle a newly received event */
                // librg_world_read(
                //     world,
                //     ID,
                //     (char *)event.packet->data,
                //     event.packet->dataLength,
                //     NULL
                // );

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }

    zpl_timer_update(&nettimer);

    return 0;
}

void network_client_update(void *data) {
    // /* send our data to the server */
    // char buffer[1024] = {0};
    // size_t buffer_length = 1024;

    // /* serialize peer's the world view to a buffer */
    // librg_world_write(
    //     world,
    //     ID,
    //     buffer,
    //     &buffer_length,
    //     NULL
    // );

    // /* create packet with actual length, and send it */
    // ENetPacket *packet = enet_packet_create(buffer, buffer_length, ENET_PACKET_FLAG_RELIABLE);
    // enet_peer_send(peer, 0, packet);
}

bool network_client_is_connected() {
    return peer ? enet_peer_get_state(peer) == ENET_PEER_STATE_CONNECTED : false;
}

static int32_t network_msg_send_raw(uint16_t  peer_id, void *data, size_t datalen, uint32_t flags) {
    ENetPacket *packet = enet_packet_create(data, datalen, flags);
    enet_peer_send(peer, 0, packet);
}

int32_t network_msg_send(uint16_t peer_id, void *data, size_t datalen) {
    network_msg_send_raw(0, data, datalen, ENET_PACKET_FLAG_RELIABLE);
}

int32_t network_msg_send_unreliable(uint16_t peer_id, void *data, size_t datalen) {
    network_msg_send_raw(0, data, datalen, 0);
}
