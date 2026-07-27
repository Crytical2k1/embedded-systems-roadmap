#include "speed_controller.h"
#include "encoder.h"
#include "motor.h"
#include "pid.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include <math.h>

//Constants
#define CONTROL_PERIOD_MS 100
#define KP 1.0f
#define KI 0.5f
#define KD 0.05f
#define PWM_MIN 0.0f
#define PWM_MAX 255.0f

typedef enum {
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} motor_direction_t;


static float target_rpm = 0.0f;
static float pwm = 0;
//static const char *TAG = "motor";
//static float error = 0;

static pid_controller motor_pid;

static void limit_pwm(void) {
    if (pwm > PWM_MAX) {
        pwm = PWM_MAX;
    }
    if (pwm < PWM_MIN) {
        pwm = PWM_MIN;
    }
}

static void speed_control(void) {
    motor_direction_t direction;
    float target = target_rpm;
    if (target >= 0) {
        direction = MOTOR_FORWARD;
    } else {
        direction = MOTOR_BACKWARD;
        target = -target;
    }
    float rpm = fabsf(encoder_get_rpm());
    
    pwm = pid_compute(
        &motor_pid,
        target,
        rpm,
        0.1f //due to the controller running at 100ms
    );
    
    limit_pwm();
    if (direction == MOTOR_FORWARD) {
        motor_forward((uint8_t)pwm);
    } else {
        motor_backward((uint8_t)pwm);
    }

    //ESP_LOGI(TAG, "Target %.1f RPM | Actual %.1f RPM | PWM %.1f", target_rpm, rpm, pwm);
}
    

static void speed_task(void *pvParameters) {
    TickType_t last_wake = xTaskGetTickCount();
    while(1) {
        // Control algorithm
        speed_control();
        // Delay
        vTaskDelayUntil(&last_wake ,pdMS_TO_TICKS(CONTROL_PERIOD_MS));
    }
}

void speed_controller_init(void) {
    pwm = 0;

    pid_init(&motor_pid, KP, KI, KD, PWM_MIN, PWM_MAX);

    xTaskCreate(
        speed_task,
        "Speed_Task",
        4096,
        NULL,
        5,
        NULL
    );
}

void speed_controller_set_target(float rpm) {
    target_rpm = rpm;
}

float speed_controller_get_target(void) {
    return target_rpm;
}

void speed_controller_stop(void) {
    target_rpm = 0;
}

