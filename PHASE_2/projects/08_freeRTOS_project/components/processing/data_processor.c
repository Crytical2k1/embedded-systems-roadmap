#include "data_processor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "math.h"
#include "esp_log.h"

static const char *TAG = "data_processor";

typedef struct{
    int16_t data;
    uint8_t sensor_id;
} raw_data_t;

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

#define RANGE_LOW_LIGHT 100
#define RANGE_LOW_TEMP 0
#define RANGE_HIGH_TEMP 50
typedef enum {
    SYSTEM_GOOD = (1 << 6),
    NTC_SENSOR_RANGE = (1 << 4),
    PHOTO_SENSOR_RANGE = (1 << 2),
    IMU_SENSOR_RANGE = (1 << 0),
} anomalies_system_bits_t; // pin out of 74HC595

//forward declaration
static uint16_t adc_raw_to_resistance(uint16_t data); 
static int16_t resistance_to_value(uint16_t resistance, uint8_t sensor_id);
static anomalies_system_bits_t check_data_range(anomalies_system_bits_t *data_status, int16_t data, uint8_t sensor_id, QueueHandle_t data_status_queue);
static void set_telemetry(telemetry_format_t *telemetry, int16_t data, uint8_t sensor_id, uint8_t status, uint32_t time_frame);
static void print_telemetry(telemetry_format_t *t);

void raw_proccesing_task(void *pvParameters) {
    QueueHandle_t queue = pvParameters;
    BaseType_t xStatus;
    raw_data_t received_value;
    telemetry_format_t telemetry;
    int16_t data_buffer = 0;
    uint32_t time_frame = 0;
    anomalies_system_bits_t data_status;

    QueueHandle_t data_status_queue = xQueueCreate(3, 1);
    esp_err_t esp_ret = xTaskCreate(data_status_task, "data_status_task", 2048, (void *)data_status_queue, DATA_STATUS_PRIO, NULL);
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create data status task");
        return;
    }
    ESP_LOGI(TAG, "Successfully create data status task.");

    while (1) {
        //receive data from queue
        xStatus = xQueueReceive(queue, &received_value, portMAX_DELAY);
        if (xStatus != pdPASS) {
            ESP_LOGE(TAG, "Failed to received data from sensor");
            continue;
        }


        if (received_value.sensor_id == IMU_SENSOR_ID) {
            continue;
            //function for euler angles
        } else {
            //get sensor resistance from adc raw value
            data_buffer = adc_raw_to_resistance(received_value.data);
            //process the data based on the sensor
            data_buffer = resistance_to_value(data_buffer, received_value.sensor_id);
        }
        data_status = check_data_range(&data_status, data_buffer, received_value.sensor_id, data_status_queue);
        set_telemetry(&telemetry, data_buffer, received_value.sensor_id, data_status, time_frame++);

        print_telemetry(&telemetry);
    }
}

static uint16_t adc_raw_to_resistance(uint16_t data) {
    uint16_t voltage = data * V_max / D_max;
    return ((VD_RESISTOR * voltage) / (V_IN - voltage));
}
static int16_t resistance_to_value(uint16_t resistance, uint8_t sensor_id) {
    if (sensor_id == NTC_SENSOR_ID) {
        //calculate temperature
        return (int16_t)((1.0f / ((1.0f/NTC_T0) + (1.0f/NTC_B)*log((float)resistance/NTC_R25))) - KELVIN_CELSIUS);
    } else if (sensor_id == PHOTO_SENSOR_ID){
        //calculate Lux
        return (int16_t)(10 * (pow((float)resistance/PHOTO_R10,-1.0f/PHOTO_GAMMA)));
    } else {
        ESP_LOGE(TAG, "Wrong sensor id, cannot get value");
    }
    return 0;
}

static anomalies_system_bits_t check_data_range(anomalies_system_bits_t *data_status, int16_t data, uint8_t sensor_id, QueueHandle_t data_status_queue) {
    if (sensor_id == NTC_SENSOR_ID) {
        if (data < RANGE_LOW_TEMP || data > RANGE_HIGH_TEMP) {
            *data_status |= NTC_SENSOR_RANGE;
        } else {
            *data_status &= ~NTC_SENSOR_RANGE;
        }
    } else if (sensor_id == PHOTO_SENSOR_ID) {
        if (data < RANGE_LOW_LIGHT) {
            *data_status |= PHOTO_SENSOR_RANGE;
        } else {
            *data_status &= ~PHOTO_SENSOR_RANGE;
        }
    }
    if (*data_status & (PHOTO_SENSOR_RANGE | NTC_SENSOR_RANGE | IMU_SENSOR_RANGE)) {
        //some of the data is out of range so the system is not good
        *data_status &= ~SYSTEM_GOOD;
    } else {
        *data_status |= SYSTEM_GOOD;
    }
    xQueueSendToBack(data_status_queue, data_status, pdMS_TO_TICKS(100));
    return *data_status;
}

static uint16_t crc16_ccitt(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF; //init
    for (uint8_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021; // polinomial
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

static void set_telemetry(telemetry_format_t *telemetry, int16_t data, uint8_t sensor_id, uint8_t status, uint32_t time_frame) {
    //sync
    telemetry->sync = SYNC_VALUE;
    //headers
    telemetry->source_id = sensor_id;
    telemetry->length = sizeof(telemetry->timestamp) + sizeof(telemetry->payload_data) + sizeof(telemetry->data_status); //32bits + 16bits + 8bits = 7bytes
    //data
    telemetry->timestamp = time_frame;
    telemetry->payload_data = data;
    telemetry->data_status = status;
    //checksum
    telemetry->crc = crc16_ccitt((uint8_t*)&telemetry->source_id, sizeof(telemetry_format_t) - sizeof(telemetry->sync) - sizeof(telemetry->crc));
}

static void print_telemetry(telemetry_format_t *t) {
    printf("SYNC: 0x%04X | ", t->sync);
    printf("SRC: %u | ", t->source_id);
    printf("LEN: %u | ", t->length);
    printf("TIME: %lu | ", t->timestamp);
    printf("DATA: %d | ", t->payload_data);
    printf("STATUS: %u | ", t->data_status);
    printf("CRC: 0x%04X\n", t->crc);
}
