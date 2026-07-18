#include "main.h"
#include "motor.h"
#include "encoder.h"
#include "speed_controller.h"
#include "imu_driver.h"
#include "imu_task.h"
#include "angle_controller.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define IMU_TASK_PRIO 5U
#define ANGLE_CONTROL_PRIO 5U

void app_main() {
    motor_enable();
    encoder_init(); //not currently used, but useful for future telemetry
    imu_init();

    imu_task_init(IMU_TASK_PRIO);
    angle_controller_init(ANGLE_CONTROL_PRIO);

    vTaskDelay(pdMS_TO_TICKS(500)); // let the complementary filter settle for a few cycles before closing the loop

    angle_controller_set_target(0.0f); //the angle to hold
    angle_controller_enable(true);
}