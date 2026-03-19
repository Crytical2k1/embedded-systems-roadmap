#include "telemetry.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "telemetry";

typedef struct {
    //start marker
    uint16_t sync;
    //header
    //uint8_t packet_id; //what kind of packet it sending
    uint8_t source_id; // sensor id
    uint8_t length; //length of the telemetry sent
    uint32_t timestamp;
    int16_t payload_data; // sensor data
    uint8_t data_status; //data out of range check
    uint16_t crc; // checksum for error detection
} telemetry_format_t;

static void print_telemetry(telemetry_format_t *t) {
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
    telemetry_format_t telemetry_buffer;

    while (1) {
        xStatus = xQueueReceive(telemetry_queue, &telemetry_buffer, portMAX_DELAY);
        if (xStatus != pdPASS) {
            ESP_LOGE(TAG, "Failed to recive data through telemetry queue");
            continue;
        }
        print_telemetry(&telemetry_buffer);
    }
}