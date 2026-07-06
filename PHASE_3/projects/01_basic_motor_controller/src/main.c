#include "main.h"
#include "motor.h"
#include "encoder.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main() {
    motor_enable();
    encoder_init();

    motor_forward(180);

    while (1) {
        printf("RPM = %.2f\n", encoder_get_rpm());

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}