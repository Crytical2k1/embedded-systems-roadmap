#include "data_processor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "math.h"
#include "esp_log.h"

static const char *TAG = "data_processor";

typedef struct{
    int16_t data[3];
    uint8_t sensor_id;
} raw_data_t;

#define NTC_SENSOR_ID 0
#define PHOTO_SENSOR_ID 1

//forward declaration
static uint16_t adc_raw_to_resistance(uint16_t data); 
static int16_t resistance_to_value(uint16_t resistance, uint8_t sensor_id);

void raw_proccesing_task(void *pvParameters) {
    QueueHandle_t queue = pvParameters;
    BaseType_t xStatus;
    raw_data_t received_value;
    int16_t data_buffer = 0;

    while (1) {
        //receive data from queue
        xStatus = xQueueReceive(queue, &received_value, portMAX_DELAY);
        if (xStatus != pdPASS) {
            ESP_LOGE(TAG, "Failed to received data from sensor");
            continue;
        }
        //get sensor resistance from adc raw value
        data_buffer = adc_raw_to_resistance(received_value.data[0]);
        //process the data based on the sensor
        data_buffer = resistance_to_value(data_buffer, received_value.sensor_id);
        //print processed data
        switch (received_value.sensor_id) {
        case NTC_SENSOR_ID:
            printf("Temp received: %d C\r\n", data_buffer);
            break;
        case PHOTO_SENSOR_ID:
            printf("Light received: %d Lux\r\n", data_buffer);
            break;
        default:
            break;
        }
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