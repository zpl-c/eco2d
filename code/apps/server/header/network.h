#pragma once

int32_t network_init(void);
int32_t network_destroy(void);

int32_t network_server_start(const char *host, uint16_t port);
int32_t network_server_stop(void);
int32_t network_server_tick(void);
void    network_server_update(void *data);

uint64_t network_client_create(void *peer);
void network_client_destroy(uint64_t ent_id);

int32_t network_msg_send(void *peer, void *data, size_t datalen);
int32_t network_msg_send_unreliable(void *peer, void *data, size_t datalen);

WORLD_PKT_WRITER(mp_pkt_writer);