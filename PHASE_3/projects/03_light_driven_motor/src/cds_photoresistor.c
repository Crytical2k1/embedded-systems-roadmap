#include "sensor_driver.h"
#include "angle_controller.h"
#include "imu_filter.h"

#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include "esp_log.h"
#include "esp_timer.h"

#define LIGHT_DEADBAND_DEG 2.0f
#define PHOTO_TASK_PERIOD_MS 100
#define IDLE_TIMEOUT_S 10.0f
#define SWEEP_COMPLETE_TOLERANCE_DEG 5.0f
#define SWEEP_TIMEOUT_S 3.0f

typedef enum {
    LIGHT_STATE_IDLE,
    LIGHT_STATE_TRACKING,
    LIGHT_STATE_SWEEPING,
    LIGHT_STATE_RETURNING
} ligth_state_t;

static const char *TAG = "photo_driver";
static const uint8_t photo_pin_1 = ADC1_CHANNEL_6;
static const uint8_t photo_pin_2 = ADC1_CHANNEL_7;

//Initialized the photo sensors
esp_err_t photo_init(void) {
    esp_err_t esp_ret;
    esp_ret = adc1_config_width(ADC_WIDTH_BIT_12);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ADC1 width");
        return ESP_FAIL;
    }
    esp_ret = adc1_config_channel_atten(photo_pin_1, ADC_ATTEN_DB_12); 
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure photoresistor 1 gpio");
        return ESP_FAIL;
    }
    esp_ret = adc1_config_channel_atten(photo_pin_2, ADC_ATTEN_DB_12); 
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure photoresistor 2 gpio");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Light sensor connected");
    return ESP_OK;
}

//Reads the values from the sensors
void photo_read(photo_reading_t *value) {
    value->sensor_1 = adc1_get_raw(photo_pin_1);
    value->sensor_2 = adc1_get_raw(photo_pin_2);
}

static void photo_task(void *pvParameters) {
    photo_reading_t value;
    ligth_state_t state = LIGHT_STATE_IDLE;
    float sweep_target = 0.0f;
    float sweep_start_heading = 0.0f;
    float sweep_start_time_us = 0.0f;
    int64_t last_light_seen_us = esp_timer_get_time();

    while (1) {
        photo_read(&value);

        float difference = (float)value.sensor_1 - (float)value.sensor_2;
        float total = (float)value.sensor_1 + (float)value.sensor_2;
        float light_error = atan2f(difference, total) * 180.0 / (float)M_PI;

        int64_t now_us = esp_timer_get_time();
        float current_heading = imu_filter_get_heading();

        //rotate just if the angle is big enough
        if (fabsf(light_error) > LIGHT_DEADBAND_DEG) {
            state = LIGHT_STATE_TRACKING;
            last_light_seen_us = now_us;
            angle_controller_set_target(current_heading + light_error);
            angle_controller_enable(true);
        } else if (state == LIGHT_STATE_SWEEPING) {
            //If nothing better is spotted yet, keep turning toward sweep_target until full rotation
            bool reached_target = fabsf(current_heading - sweep_target) < SWEEP_COMPLETE_TOLERANCE_DEG;
            bool timed_out = (now_us - sweep_start_time_us) / 1000000.0f > SWEEP_TIMEOUT_S;

            if(reached_target || timed_out) {
                //full turn done and nothing better found
                state = LIGHT_STATE_RETURNING;
                angle_controller_set_target(sweep_start_heading);
                angle_controller_enable(true);
            }
        } else if (state == LIGHT_STATE_RETURNING) {
            if (fabsf(current_heading - sweep_start_heading) < SWEEP_COMPLETE_TOLERANCE_DEG) {
                state = LIGHT_STATE_IDLE;
                last_light_seen_us = now_us;
                angle_controller_enable(false);
            }
        } else {
            //hold still unless it has been long enough to do a scan
            float idle_s = (now_us - last_light_seen_us) / 1000000.0f;
            if (idle_s > IDLE_TIMEOUT_S) {
                state = LIGHT_STATE_SWEEPING;
                sweep_start_heading = current_heading;
                sweep_target = current_heading + 360.0f;
                sweep_start_time_us = now_us;
                angle_controller_set_target(sweep_target);
                angle_controller_enable(true);
            } else {
                angle_controller_enable(false);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(PHOTO_TASK_PERIOD_MS));
    }
}

void photo_tracking_init(uint8_t prio) {
    esp_err_t ret = photo_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Photo sensor init failed");
        abort();
    }

    BaseType_t task_created = xTaskCreate(photo_task, "PhotoTask", 2560, NULL, prio, NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create photo tracking task");
        abort();
    }
}