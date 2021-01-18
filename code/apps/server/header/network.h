#pragma once

int32_t network_init(void);
int32_t network_destroy(void);

int32_t network_server_start(const char *host, uint16_t port);
int32_t network_server_stop(void);
int32_t network_server_tick(void);
void    network_server_update(void *data);

uint64_t network_client_create(uint16_t peer_id);
void network_client_destroy(uint64_t ent_id);
