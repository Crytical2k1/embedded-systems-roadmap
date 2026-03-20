#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_wifi.h"
#include "esp_http_server.h"
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
} telemetry_wifi_t;

extern telemetry_wifi_t latest_telemetry;
extern SemaphoreHandle_t telemetry_mutex;

void telemetry_send_task(void *pvParameters);
void wifi_init_sta(void);

#endif