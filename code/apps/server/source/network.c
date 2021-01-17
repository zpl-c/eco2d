#include "zpl.h"

#define ENET_IMPLEMENTATION
#include "enet.h"

#define LIBRG_IMPL
#define LIBRG_CUSTOM_ZPL
#include "librg.h"

#include "system.h"
#include "network.h"

#define NETWORK_UPDATE_DELAY 0.100
#define NETWORK_MAX_CLIENTS 32

static ENetHost *server = NULL;
static librg_world *server_world = NULL;
static zpl_timer nettimer = {0};

int32_t network_init(void) {
    zpl_timer_set(&nettimer, NETWORK_UPDATE_DELAY, -1, network_server_update);
    return enet_initialize() != 0;
}

int32_t network_destroy(void) {
    enet_deinitialize();

    return 0;
}

int32_t server_write_update(librg_world *w, librg_event *e) {
    int64_t owner_id = librg_event_owner_get(w, e);
    int64_t entity_id = librg_event_entity_get(w, e);

    return 0;

    // /* prevent sending updates to users who own that entity */
    // /* since they will be responsible on telling where that entity is supposed to be */
    // if (librg_entity_owner_get(w, entity_id) == owner_id) {
    //     return LIBRG_WRITE_REJECT;
    // }

    // /* read our current position */
    // ENetPeer *peer = (ENetPeer *)librg_entity_userdata_get(w, entity_id);

    // char *buffer = librg_event_buffer_get(w, e);
    // size_t max_length = librg_event_size_get(w, e);

    // /* check if we have enough space to write and valid position */
    // if (sizeof(vec3) > max_length || !peer->data) {
    //     return LIBRG_WRITE_REJECT;
    // }

    // /* write data and return how much we've written */
    // memcpy(buffer, peer->data, sizeof(vec3));
    // return sizeof(vec3);
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
    server_world = librg_world_create();

    if (server_world == NULL) {
        zpl_printf("[ERROR] An error occurred while trying to create a server world.\n");
        return 1;
    }

    zpl_printf("[INFO] Created a new server world\n");

    /* store our host to the userdata */
    librg_world_userdata_set(server_world, server);

    /* config our world grid */
    librg_config_chunksize_set(server_world, 16, 16, 16);
    librg_config_chunkamount_set(server_world, 9, 9, 9);
    librg_config_chunkoffset_set(server_world, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID);

    librg_event_set(server_world, LIBRG_WRITE_UPDATE, server_write_update);
    // librg_event_set(server_world, LIBRG_READ_UPDATE, server_read_update);

    zpl_timer_start(&nettimer, NETWORK_UPDATE_DELAY);

    return 0;
}

int32_t network_server_stop(void) {
    zpl_timer_stop(&nettimer);
    enet_host_destroy(server);
    librg_world_destroy(server_world);

    server_world = NULL;
    server = NULL;

    return 0;
}

int32_t network_server_tick(void) {
    ENetEvent event = {0};
    while (enet_host_service(server, &event, 1) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                zpl_printf("[INFO] A new user %d connected.\n", event.peer->incomingPeerID);
                int64_t entity_id = event.peer->incomingPeerID;

                // /* we create an entity for our client */
                // /* in our case it is going to have same id as owner id */
                // /* since we do not really plan on adding more entities per client for now */
                // /* and place his entity right in the centerl of the world */
                // librg_entity_track(server_world, entity_id);
                // librg_entity_owner_set(server_world, entity_id, event.peer->incomingPeerID);
                // librg_entity_chunk_set(server_world, entity_id, 1);
                // librg_entity_radius_set(server_world, entity_id, 2); /* 2 chunk radius visibility */
                // librg_entity_userdata_set(server_world, entity_id, event.peer); /* save ptr to peer */
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                zpl_printf("[INFO] A user %d disconnected.\n", event.peer->incomingPeerID);
                int64_t entity_id = event.peer->incomingPeerID;
                // librg_entity_untrack(server_world, entity_id);
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                // /* handle a newly received event */
                // librg_world_read(
                //     server_world,
                //     event.peer->incomingPeerID,
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

void network_server_update(void *data) {
    // /* iterate peers and send them updates */
    // ENetPeer *currentPeer;
    // for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
    //     if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
    //         continue;
    //     }

    //     char buffer[1024] = {0};
    //     size_t buffer_length = 1024;

    //     /* serialize peer's the world view to a buffer */
    //     librg_world_write(
    //         server_world,
    //         currentPeer->incomingPeerID,
    //         buffer,
    //         &buffer_length,
    //         NULL
    //     );

    //     /* create packet with actual length, and send it */
    //     ENetPacket *packet = enet_packet_create(buffer, buffer_length, ENET_PACKET_FLAG_RELIABLE);
    //     enet_peer_send(currentPeer, 0, packet);
    // }
}
