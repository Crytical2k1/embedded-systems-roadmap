#include "imu_task.h"
#include "imu_driver.h"
#include "lcd_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//helpers

//forward declaration
static void imu_task(void *pvParameters);

void imu_task_init(void) {

    xTaskCreate(imu_task, "ImuTask", 2048, NULL, 5, NULL);
}

static void imu_task(void *pvParameters) {
    int16_t accel_buffer[3];
    char line[32]; // buffer for formatted string


    while (1) {
        imu_read(accel_buffer);

        snprintf(line, sizeof(line), "%d,%d,%d", accel_buffer[0], accel_buffer[1], accel_buffer[2]);
        lcd_manager_write(0,0, line);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
