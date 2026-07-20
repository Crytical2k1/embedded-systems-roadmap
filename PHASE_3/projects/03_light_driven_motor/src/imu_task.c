#include "imu_task.h"
#include "imu_driver.h"
#include "imu_filter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

typedef enum {
    AXIS_X = 0,
    AXIS_Y,
    AXIS_Z,
    AXIS_COUNT
} axis_t;
static int16_t accel_offset[AXIS_COUNT] = {0};
static int16_t gyro_offset[AXIS_COUNT] = {0};
#define CALIBRATION_ITERATIONS 1000
#define GRAVITY_FACTOR 16384
#define IMU_LOOP_PERIOD_MS 10

static const char *TAG = "imu_task";
//forward declaration
static void imu_task(void *pvParameters);
static void sensor_calibration(void);

void imu_task_init(uint8_t prio) {
    imu_filter_init();
    BaseType_t task_created = xTaskCreate(imu_task, "ImuTask", 4096, NULL, prio, NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create IMU task");
        abort();
    }
}

static int16_t apply_calibration(int16_t value, const int16_t offset[AXIS_COUNT], uint8_t axis) {
    return value - offset[axis];
}

static void imu_task(void *prParameters) {
    //calibrate first the sensor
    sensor_calibration();

    int16_t accel_buffer[AXIS_COUNT];
    int16_t gyro_buffer[AXIS_COUNT];
    int16_t accel[AXIS_COUNT];
    int16_t gyro[AXIS_COUNT];

    int64_t last_time_us = esp_timer_get_time();
    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        imu_read(accel_buffer, gyro_buffer);

        for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
            accel[axis] = apply_calibration(accel_buffer[axis], accel_offset, axis);
            gyro[axis] = apply_calibration(gyro_buffer[axis], gyro_offset, axis);
        }

        int64_t now_us = esp_timer_get_time();
        float dt_s = (now_us - last_time_us) / 1000000.0f;
        last_time_us = now_us;
        if (dt_s <= 0.0f) {
            dt_s = IMU_LOOP_PERIOD_MS / 1000.0f;
        }

        imu_filter_update(accel, gyro, dt_s);
        
        //For debuggin purposes only
        //ESP_LOGI(TAG, "angle=%.2f rate=%.2f", imu_filter_get_angle(), imu_filter_get_rate());

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(IMU_LOOP_PERIOD_MS));
    }
}

static void sensor_calibration(void) {
    int32_t sum_accel[AXIS_COUNT] = {0};
    int32_t sum_gyro[AXIS_COUNT] = {0};
    //take measurements and add them up
    for (uint16_t i = 0; i < CALIBRATION_ITERATIONS; i++) {
        int16_t a[AXIS_COUNT];
        int16_t g[AXIS_COUNT];
        imu_read(a,g);

        for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
            sum_accel[axis] += a[axis];
            sum_gyro[axis] += g[axis];
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    //divide the sums by the number of measurements to get the mean
    for (uint8_t axis = 0; axis < AXIS_COUNT; axis++) {
        accel_offset[axis] = sum_accel[axis] / CALIBRATION_ITERATIONS;
        gyro_offset[axis] = sum_gyro[axis] / CALIBRATION_ITERATIONS;
    }
    accel_offset[AXIS_Z] -= GRAVITY_FACTOR;
    ESP_LOGI(TAG, "Sensor Calibrated");
}
