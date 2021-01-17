#pragma once

int32_t network_init(void);
int32_t network_destroy(void);

int32_t network_server_start(const char *host, uint16_t port);
int32_t network_server_stop(void);
int32_t network_server_tick(void);
void    network_server_update(void *data);
