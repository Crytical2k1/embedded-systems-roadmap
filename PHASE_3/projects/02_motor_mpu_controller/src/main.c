#include "main.h"
#include "motor.h"
#include "encoder.h"
#include "speed_controller.h"
#include "imu_driver.h"
#include "imu_task.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define IMU_TASK_PRIO 5U

void app_main() {
    motor_enable();
    encoder_init();
    speed_controller_init();
    imu_init();

    imu_task_init(IMU_TASK_PRIO);

    // while (1) {

    //     speed_controller_set_target(150);

    //     vTaskDelay(pdMS_TO_TICKS(5000));

    //     speed_controller_stop();

    //     vTaskDelay(pdMS_TO_TICKS(1000));

    //     speed_controller_set_target(-150);

    //     vTaskDelay(pdMS_TO_TICKS(5000));
    // }
}