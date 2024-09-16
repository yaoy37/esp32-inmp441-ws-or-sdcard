
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_websocket_client.h"
#include "test_server/t.h"
// 如果没有esp_websocket_client 库可以在项目目录里执行 下面命令指定依赖，或者到官网（git）下载最新的代码
// idf.py add-dependency "espressif/esp_websocket_client=*"

#define WEBSOCKET_URI "ws://127.0.0.1:8009/ws_binary"  // WebSocket 服务器地址

static const char *TAG = "WEBSOCKET_CLIENT";


esp_websocket_client_handle_t client;

void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WebSocket connected");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WebSocket disconnected");
            break;
        case WEBSOCKET_EVENT_DATA:
            // ESP_LOGI(TAG, "Received data");
            // printf("Received opcode=%d\n", data->op_code);
            if (data->op_code == 0x1) {
                // printf("Received text data: %.*s\n", data->data_len, (char *)data->data_ptr);
            }
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WebSocket error");
            break;
    }
}

void websocket_app_start(void) {
    // 配置 WebSocket 客户端
    esp_websocket_client_config_t websocket_cfg = {
        .uri = WEBSOCKET_URI,
        .headers = "X-Client-ID: client-id-12345\r\n",
    };

    // 初始化 WebSocket 客户端
    client = esp_websocket_client_init(&websocket_cfg);

    // 注册事件处理器
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

    // 启动 WebSocket 客户端并连接到服务器
    ESP_ERROR_CHECK(esp_websocket_client_start(client));

}

void send_ws_bin(char*data,int64_t data_size){
    if(!client){
        ESP_LOGI(TAG, "start websocket_app");
        websocket_app_start();
    }
    if (!esp_websocket_client_is_connected(client)) {
        // 关闭 WebSocket 连接
        esp_websocket_client_stop(client);
        esp_websocket_client_destroy(client);
        client = NULL;
        ESP_LOGI(TAG, "reset websocket_app");
        websocket_app_start();
    };
    // test_socket();
    esp_websocket_client_send_bin(client, data, data_size, 5000);
}
