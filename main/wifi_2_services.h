#ifndef WIFI_S
#define WIFI_S
#include "esp_websocket_client.h"

extern int8_t smartconfig_over;
esp_websocket_client_handle_t websocket_client_init(void);
void initialise_wifi(void);

#endif // WIFI_H
