#ifndef WS_SEND_H
#define WS_SEND_H


#include "esp_websocket_client.h"

extern esp_websocket_client_handle_t client;

void send_ws_bin(char *data,int64_t data_size);

#endif