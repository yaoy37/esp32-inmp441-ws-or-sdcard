idf_component_register(
    SRCS "i2s_recorder_main.c" "web_socket/ws_send.c" "mqtt_server/my_send_mqtt.c" "wifi_server/wifi_2_services.c" "test_server/t.c"
    INCLUDE_DIRS "." 
    # REQUIRES  esp_websocket_client
    )
