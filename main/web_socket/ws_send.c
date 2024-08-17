#include <stdio.h>
#include "esp_websocket_client.h"
#include "ws_send.h"
esp_websocket_client_handle_t websocket_client = NULL;

static const char *TAG = "ws_send";
void send_ws(char *data, int data_size)
{
    if (websocket_client == NULL)
    {
        websocket_client = websocket_client_init();
    };
    if (!esp_websocket_client_is_connected(websocket_client))
    {
        esp_websocket_client_stop(websocket_client);
        esp_websocket_client_destroy(websocket_client);
        websocket_client = NULL;
        websocket_client = websocket_client_init();
    }
    esp_websocket_client_send_bin(websocket_client, data, data_size, 5000);
}
