/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_eap_client.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "esp_mac.h"
#include "wifi_2_services.h"


/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;
int8_t smartconfig_over = 0;
int8_t if_got_ssid = 0;
/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "wifi_2_s";

// esp_websocket_client_handle_t websocket_client = NULL;
static void smartconfig_example_task(void * parm);

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
// --------------**事件触发和处理流程
// 1.初始化 Wi-Fi:
// initialise_wifi 方法被调用，初始化网络接口、创建事件循环、设置事件处理程序。
// 调用 esp_wifi_start 后，Wi-Fi 驱动启动并触发 WIFI_EVENT_STA_START 事件。

// 2.处理 WIFI_EVENT_STA_START 事件:
// 事件循环机制捕捉到 WIFI_EVENT_STA_START 事件并调用 event_handler 处理。
// 在 event_handler 中，当 event_base 为 WIFI_EVENT 且 event_id 为 WIFI_EVENT_STA_START 时，启动 SmartConfig 任务。

// 3.启动 SmartConfig 任务:
// xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
// SmartConfig 任务启动后，设备进入等待状态，准备接收手机发送的 Wi-Fi 配置信息。

// 4.手机发送 Wi-Fi 配置信息:
// 手机通过 ESP Touch 协议发送 Wi-Fi 的 SSID 和密码。
// 设备通过 SmartConfig 接收并解析这些信息，触发 SC_EVENT_GOT_SSID_PSWD 事件。

// 5.处理 SC_EVENT_GOT_SSID_PSWD 事件:
// 事件循环机制捕捉到 SC_EVENT_GOT_SSID_PSWD 事件并调用 event_handler 处理。
// 在 event_handler 中，当 event_base 为 SC_EVENT 且 event_id 为 SC_EVENT_GOT_SSID_PSWD 时，设备使用接收到的 SSID 和密码配置 Wi-Fi，并尝试连接到网络。

// 6.连接 Wi-Fi 并获取 IP:
// 设备成功连接到 Wi-Fi 网络并获取 IP 地址，触发 IP_EVENT_STA_GOT_IP 事件。

// 7.处理 IP_EVENT_STA_GOT_IP 事件:
// 事件循环机制捕捉到 IP_EVENT_STA_GOT_IP 事件并调用 event_handler 处理。
// 在 event_handler 中，当 event_base 为 IP_EVENT 且 event_id 为 IP_EVENT_STA_GOT_IP 时，标记连接成功，设备可以开始与服务器通信。
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        if(if_got_ssid) return;
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if(if_got_ssid) return;
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        if(if_got_ssid) return;
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        // websocket_client = websocket_client_init(); // wifi 连接成功后，启动 websocket 服务
        // esp_smartconfig_stop();
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        if(if_got_ssid) return;
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        if(if_got_ssid) return;
        ESP_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");
        if(if_got_ssid) return;
        if_got_ssid = 1;
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:");
            for (int i=0; i<33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        esp_wifi_connect();
        vTaskDelay(2000);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );


    // // 等待 WiFi 连接成功
    // EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    // if (bits & CONNECTED_BIT) {
    //     esp_smartconfig_stop();
    //     ESP_LOGI(TAG, "WiFi connected successfully");
    // } else {
    //     ESP_LOGE(TAG, "Failed to connect to WiFi");
    //     return;
    // }

}


static void smartconfig_example_task(void * parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            smartconfig_over = 1;
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}

// void wifi_2_s(void)
// {
//     ESP_ERROR_CHECK( nvs_flash_init() );
//     initialise_wifi();
// }
