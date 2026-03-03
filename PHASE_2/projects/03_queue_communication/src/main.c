#include "sensor_driver.h"
#include "sensor_task.h"
#include "controller_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void app_main() {
    //initializing sensor
    printf("Starting IMU communication...\r\n");
    imu_init();

    //queue creating
    QueueHandle_t imu_queue = xQueueCreate(5, sizeof(int16_t[3]));
    if (imu_queue == NULL) {
        printf("Error: failed creating the queue");
        return;
    }

    //initialize tasks
    controller_task_init(imu_queue);
    sensor_task_init(imu_queue);
}
