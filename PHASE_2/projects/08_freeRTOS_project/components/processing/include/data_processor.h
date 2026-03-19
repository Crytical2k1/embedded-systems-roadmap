#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define DATA_STATUS_PRIO 10

//helpers
#define NTC_SENSOR_ID 0
#define PHOTO_SENSOR_ID 1
#define IMU_SENSOR_ID 2

typedef struct {
    QueueHandle_t raw_queue;
    QueueHandle_t telemetry_queue;
} queue_params_t;

// Telemetry
#define SYNC_VALUE 0xAA55
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

//refactor constants
#define VD_RESISTOR 5100U //5.1kOhm known resistance of voltage divider
#define V_IN 3300 //3.3V  vref for voltage divider
#define V_max 2450 //2.45V value for adc raw to voltage
#define D_max 4095 // value for adc raw to voltage
//NTC constants
#define NTC_T0 298.15f // reference temperature 25ºC
#define NTC_B 3950.0f // beta value
#define NTC_R25 10000.0f //resistance at 25ºC, 10kOhm
#define KELVIN_CELSIUS 273.15f
//Photoresistor constants
#define PHOTO_R10 30000.0f // resistance at 10Lux, 30kOhm (dont know)
#define PHOTO_GAMMA (float)0.7 // gamma value


void raw_proccesing_task(void *pvParameters);
void data_status_task(void *pvParameters);

#endif