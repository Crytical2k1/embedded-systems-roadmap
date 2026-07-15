#include "angle_controller.h"
#include "imu_filter.h"
#include "motor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define CONTROL_PERIOD_MS 10

#define KP 15.0f
#define KI 0
#define KD 0.8f

#define OUTPUT_MAX 255.0f
#define OUTPUT_MIN -255.0f
#define INTEGRAL_LIMIT 50.0f

static const char *TAG = "angle_control";
static float target_angle = 0.0f;
static float integral = 0.0f;
static volatile bool controller_enabled = false;

static void angle_task(void *pvParameters) {
    TickType_t last_wake = xTaskGetTickCount();
    const float dt_s = CONTROL_PERIOD_MS / 1000.0f;

    while (1) {
        if (controller_enabled) {
            float current_angle = imu_filter_get_angle();
            float rate = imu_filter_get_rate(); //deg/s, straight from the gyro
            float error = target_angle - current_angle;

            //calculates the integral value and checks if it is inside the limits
            integral += error * dt_s;
            if (integral > INTEGRAL_LIMIT) integral = INTEGRAL_LIMIT;
            if (integral < -INTEGRAL_LIMIT) integral = -INTEGRAL_LIMIT;

            //calculates the PID value
            float output = KP * error + KI * integral - KD * rate;
            if (output > OUTPUT_MAX) output = OUTPUT_MAX;
            if (output < OUTPUT_MIN) output = OUTPUT_MIN;

            //rotates the motor
            if (output >= 0.0f) {
                motor_forward((uint16_t)output);
            } else {
                motor_backward((uint16_t)(-output));
            }
        } else {
            // stops the motor 
            motor_stop();
            integral = 0.0f;
        }
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(CONTROL_PERIOD_MS));
    }
}

void angle_controller_init(uint8_t prio) {
    BaseType_t task_created = xTaskCreate(angle_task, "AngleTask", 4096, NULL, prio, NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create the angle controller task");
        abort();
    }
}

void angle_controller_set_target(float angle_deg) {
    target_angle = angle_deg;
}
float angle_controller_get_target(void) {
    return target_angle;
}
void angle_controller_enable(bool enable) {
    controller_enabled = enable;
}