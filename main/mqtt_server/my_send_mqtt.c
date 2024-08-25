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

esp_mqtt_client_handle_t mqtt_client = NULL;
static void mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    static int32_t last_id = 0;
    int msg_id;

    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(event->client, TOPIC, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_DATA:
        if (!strcmp(event->topic, TOPIC))
        {
            ESP_LOGI(TAG, "Received message: %.*s", event->data_len, event->data);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_mqtt_client_handle_t init_mqtt()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    return client;
}

void send_mqtt(char *data, int data_size)
{
    if (mqtt_client == NULL)
    {
        mqtt_client = init_mqtt();
    }
    esp_mqtt_client_publish(mqtt_client, TOPIC, data, data_size, 1, 0);
}
