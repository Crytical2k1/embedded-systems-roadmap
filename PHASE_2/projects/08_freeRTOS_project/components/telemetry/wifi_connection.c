#include "telemetry.h"

#include "nvs_flash.h"
#include "esp_log.h"

httpd_handle_t server = NULL;
SemaphoreHandle_t telemetry_mutex = NULL;
static const char *TAG = "Wifi_connection";

//forward declaration
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

void wifi_init_sta(void) {
    //initialize telemetry_mutex
    telemetry_mutex = xSemaphoreCreateMutex();
    if (telemetry_mutex == NULL) {
        ESP_LOGE(TAG, "failed to create telemetry mutex");
    }
    ESP_LOGI(TAG, "telemetry mutex created");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSD,
            .password = WIFI_PASSWORD
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

static httpd_handle_t start_webserver(void){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);
    return server;
}
static esp_err_t index_handler(httpd_req_t *req){
    const char* html =
        "<!DOCTYPE html>"
        "<html>"
        "<body>"
        "<h1>ESP32 Sensor Data</h1>"
        "<p id='data'>Loading...</p>"
        "<script>"
        "setInterval(async () => {"
        "  const res = await fetch('/data');"
        "  const text = await res.text();"
        "  document.getElementById('data').innerText = text;"
        "}, 1000);"
        "</script>"
        "</body>"
        "</html>";

    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t data_handler(httpd_req_t *req){
    telemetry_wifi_t copy = {0};
    char response[100];
    //copy data
    if (xSemaphoreTake(telemetry_mutex, pdMS_TO_TICKS(100))) {
        copy = latest_telemetry;
        xSemaphoreGive(telemetry_mutex);
    }

    sprintf(response, "SYNC: 0x%04X | SRC: %u | LEN: %u | TIME: %lu | DATA: %d | STATUS: %u | CRC: 0x%04X\n", 
            copy.sync, copy.source_id, copy.length, copy.timestamp, copy.payload_data, copy.data_status, copy.crc);

    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void register_handlers(httpd_handle_t server){
    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler
    };

    httpd_uri_t data_uri = {
        .uri = "/data",
        .method = HTTP_GET,
        .handler = data_handler
    };

    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &data_uri);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

        ESP_LOGI(TAG, "IP: http://" IPSTR, IP2STR(&event->ip_info.ip));

        if (server == NULL) {
            server = start_webserver();
            register_handlers(server);
        }
    }
}

