#pragma once
#include "system.h"

int32_t network_init(void);
int32_t network_destroy(void);

int32_t network_client_connect(const char *host, uint16_t port);
int32_t network_client_disconnect(void);
int32_t network_client_tick(void);
void    network_client_update(void *data);
bool    network_client_is_connected();

int32_t network_msg_send(void *data, size_t datalen);
int32_t network_msg_send_unreliable(void *data, size_t datalen);
