idf_component_register(
    SRCS "telemetry.c" "wifi_connection.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi esp_event esp_netif nvs_flash esp_http_server
)