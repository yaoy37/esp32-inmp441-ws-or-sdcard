#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "my_send_mqtt.h"
// #include "protocol_examples_common.h"
#include "mqtt_client.h"

#define TOPIC "voice"
#define TAG "send_mqtt"
#define BROKER_URI "mqtt://119.91.201.153:1883" // 替换为你的 MQTT Broker URI

esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;

        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void init_mqtt()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}
// void send_mqtt(const char *message)

void send_mqtt(char *data, int data_size)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "MQTT client is not initialized");
        init_mqtt();
        return;
    }

    int msg_id = esp_mqtt_client_publish(client, TOPIC, data, data_size, 1, 0);
    ESP_LOGI(TAG, "Message published with msg_id=%d, size=%d", msg_id, data_size);
}

// void app_main(void)
// {
//     // Initialize MQTT client
//     init_mqtt();

//     // Wait for the client to connect
//     vTaskDelay(3000 / portTICK_PERIOD_MS);

//     // Send a message to the broker
//     send_message("Hello, MQTT!");

//     // Optionally, keep the task running to handle any events
//     while (true) {
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }
