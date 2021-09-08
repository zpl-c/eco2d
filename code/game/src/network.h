#pragma once
#include "system.h"

int32_t network_init(void);
int32_t network_destroy(void);

// NOTE(zaklaus): client
int32_t network_client_connect(const char *host, uint16_t port);
int32_t network_client_disconnect(void);
int32_t network_client_tick(void);
void    network_client_update(void *data);
bool    network_client_is_connected();

// NOTE(zaklaus): server
int32_t network_server_start(const char *host, uint16_t port);
int32_t network_server_stop(void);
int32_t network_server_tick(void);
void    network_server_assign_entity(void *peer_id, uint64_t ent_id);
uint64_t network_server_get_entity(void *peer_id);

// NOTE(zaklaus): messaging
int32_t network_msg_send(void *peer_id, void *data, size_t datalen);
int32_t network_msg_send_unreliable(void *peer_id, void *data, size_t datalen);
