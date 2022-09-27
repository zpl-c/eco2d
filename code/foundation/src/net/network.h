#pragma once
#include "platform/system.h"

int32_t network_init(void);
int32_t network_destroy(void);

// NOTE(zaklaus): client
int32_t network_client_connect(const char *host, uint16_t port);
int32_t network_client_disconnect(void);
int32_t network_client_tick(void);
void    network_client_update(void *data);
bool    network_client_is_connected();

typedef struct {
    // NOTE(zaklaus):  persistent stats bytes
    uint32_t incoming_total;
    uint64_t total_received;
    uint32_t outgoing_total;
    uint64_t total_sent;

    // NOTE(zaklaus): bandwidth (bytes/sec)
    float incoming_bandwidth;
    float outgoing_bandwidth;

    // NOTE(zaklaus): packet integrity
    uint64_t packets_sent;
    uint32_t packets_lost;
    float packet_loss;

    // NOTE(zaklaus): ping
    uint32_t ping;
    uint32_t low_ping;
} network_client_stats;

network_client_stats
network_client_fetch_stats(void);

// NOTE(zaklaus): server
int32_t network_server_start(const char *host, uint16_t port);
int32_t network_server_stop(void);
int32_t network_server_tick(void);
void   network_server_despawn_viewers(void *peer_id);
uint64_t network_server_get_entity(void *peer_id, uint16_t view_id);

// NOTE(zaklaus): messaging
int32_t network_msg_send(void *peer_id, void *data, size_t datalen, uint16_t channel_id);
int32_t network_msg_send_unreliable(void *peer_id, void *data, size_t datalen, uint16_t channel_id);
