/* I2S Digital Microphone Recording Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "sdkconfig.h"
#include "nvs_flash.h"
#include "wifi_server/wifi_2_services.h"
#include "web_socket/ws_send.h"
#include "mqtt_server/my_send_mqtt.h"


static const char *TAG = "pdm_rec_example";

#define SPI_DMA_CHAN SPI_DMA_CH_AUTO
#define NUM_CHANNELS (1) // For mono recording only!
#define SAMPLE_SIZE (CONFIG_EXAMPLE_BIT_SAMPLE * 1024)
#define BYTE_RATE (CONFIG_EXAMPLE_SAMPLE_RATE * (CONFIG_EXAMPLE_BIT_SAMPLE / 8)) * NUM_CHANNELS

static int16_t i2s_buff[1024] = {0};
static size_t bytes_read;

void init_microphone(void)
{
    // Set the I2S configuration as PDM and 16bits per sample
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX, // | I2S_MODE_PDM,
        .sample_rate = CONFIG_EXAMPLE_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
        .dma_buf_count = 10,
        .dma_buf_len = 1024,
        .use_apll = 0,
    };

    // Set the pinout configuration (set using menuconfig)
    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = CONFIG_EXAMPLE_I2S_CLK_GPIO,
        .ws_io_num = CONFIG_EXAMPLE_I2S_WS_GPIO,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = CONFIG_EXAMPLE_I2S_DATA_GPIO,
    };

    // Call driver installation function before any I2S R/W operation.
    ESP_ERROR_CHECK(i2s_driver_install(CONFIG_EXAMPLE_I2S_CH, &i2s_config, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_pin(CONFIG_EXAMPLE_I2S_CH, &pin_config));
    ESP_ERROR_CHECK(i2s_set_clk(CONFIG_EXAMPLE_I2S_CH, CONFIG_EXAMPLE_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO));
}

void check_memory_usage()
{
    size_t free_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);                   // 获取默认堆的可用内存大小
    size_t largest_free_block = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT); // 获取默认堆的最大可用块大小
    size_t min_free_size = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT);       // 获取默认堆的最小可用块大小

    printf("Free memory: %u bytes\n", free_size);
    printf("Largest free block: %u bytes\n", largest_free_block);
    printf("Minimum free size: %u bytes\n", min_free_size);

    // heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
}

// int smartconfig_over1;
void read_sound_task(void)
{
    while (1)
    {
        if (smartconfig_over == 0)
        {
            vTaskDelay(2000);
            continue;
        }
        i2s_read(CONFIG_EXAMPLE_I2S_CH, (char *)i2s_buff, sizeof(i2s_buff), &bytes_read, 5000);
        if (bytes_read > 0)
        {
            send_ws_bin((char *)i2s_buff, sizeof(i2s_buff));
            // send_mqtt((char *)i2s_buff, sizeof(i2s_buff));
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "PDM microphone recording Example start ------------------- start");
    init_microphone();
    // init flash
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi();
    vTaskDelay(5000);
    // 创建 read_sound 任务
    // xTaskCreate(&read_sound_task, "read_sound_task", 1024*4, NULL, 5, NULL);
    read_sound_task();
    // Stop I2S driver and destroy
    // ESP_ERROR_CHECK( i2s_driver_uninstall(CONFIG_EXAMPLE_I2S_CH) );
}
