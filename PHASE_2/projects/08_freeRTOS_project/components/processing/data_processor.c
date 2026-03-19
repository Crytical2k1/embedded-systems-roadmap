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

#define RANGE_LOW_LIGHT 100
#define RANGE_LOW_TEMP 0
#define RANGE_HIGH_TEMP 50
typedef enum {
    SYSTEM_GOOD = (1 << 6),
    NTC_SENSOR_RANGE = (1 << 4),
    PHOTO_SENSOR_RANGE = (1 << 2),
    IMU_SENSOR_RANGE = (1 << 0),
} anomalies_system_bits_t; // pin out of 74HC595

volatile uint8_t latest_data_status = 0;

//forward declaration
static uint16_t adc_raw_to_resistance(uint16_t data); 
static int16_t resistance_to_value(uint16_t resistance, uint8_t sensor_id);
static anomalies_system_bits_t check_data_range(anomalies_system_bits_t *data_status, int16_t data, uint8_t sensor_id, TaskHandle_t task_handle);
static void set_telemetry(telemetry_format_t *telemetry, int16_t data, uint8_t sensor_id, uint8_t status, uint32_t time_frame);

void raw_proccesing_task(void *pvParameters) {
    queue_params_t *params = (queue_params_t *)pvParameters;
    QueueHandle_t raw_queue = params->raw_queue;
    QueueHandle_t telemetry_queue = params-> telemetry_queue;
    TaskHandle_t data_status_task_handle;

    BaseType_t xStatus;
    raw_data_t received_value;
    telemetry_format_t telemetry;

    int16_t data_buffer = 0;
    uint32_t time_frame = 0;
    anomalies_system_bits_t data_status;
    // Data status initialization
    esp_err_t esp_ret = xTaskCreate(data_status_task, "data_status_task", 2048, NULL, DATA_STATUS_PRIO, &data_status_task_handle);
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create data status task");
        return;
    }
    ESP_LOGI(TAG, "Successfully create data status task.");

    while (1) {
        //receive data from queue
        xStatus = xQueueReceive(raw_queue, &received_value, portMAX_DELAY);
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
        data_status = check_data_range(&data_status, data_buffer, received_value.sensor_id, data_status_task_handle);
        set_telemetry(&telemetry, data_buffer, received_value.sensor_id, data_status, time_frame++);

        //send telemetry to telemetry handler module
        xQueueSendToBack(telemetry_queue, &telemetry, pdMS_TO_TICKS(100));
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

static anomalies_system_bits_t check_data_range(anomalies_system_bits_t *data_status, int16_t data, uint8_t sensor_id, TaskHandle_t task_handle) {
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
    latest_data_status = *data_status;
    xTaskNotifyGive(task_handle);
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
