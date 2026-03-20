#include "telemetry.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "telemetry";
telemetry_wifi_t latest_telemetry = {0};


static void print_telemetry(telemetry_wifi_t *t) {
    printf("SYNC: 0x%04X | ", t->sync);
    printf("SRC: %u | ", t->source_id);
    printf("LEN: %u | ", t->length);
    printf("TIME: %lu | ", t->timestamp);
    printf("DATA: %d | ", t->payload_data);
    printf("STATUS: %u | ", t->data_status);
    printf("CRC: 0x%04X\n", t->crc);
}

void telemetry_send_task(void *pvParameters) {
    QueueHandle_t telemetry_queue = pvParameters;
    BaseType_t xStatus;
    telemetry_wifi_t telemetry_buffer;

    while (1) {
        xStatus = xQueueReceive(telemetry_queue, &telemetry_buffer, portMAX_DELAY);
        if (xStatus != pdPASS) {
            ESP_LOGE(TAG, "Failed to recive data through telemetry queue");
            continue;
        }
        if (xSemaphoreTake(telemetry_mutex, pdMS_TO_TICKS(100))) {
            latest_telemetry = telemetry_buffer;
            xSemaphoreGive(telemetry_mutex);
        }

        print_telemetry(&telemetry_buffer);
    }
}