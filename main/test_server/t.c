#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "esp_log.h"

#define TARGET_IP "192.168.10.172"
#define TARGET_PORT 8989
static const char *TAG = "TCP_TEST";

void test_socket(void)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(TARGET_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(TARGET_PORT);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", TARGET_IP, TARGET_PORT);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
    } else {
        ESP_LOGI(TAG, "Successfully connected to %s:%d", TARGET_IP, TARGET_PORT);
    }

    shutdown(sock, 0);
    close(sock);
}
