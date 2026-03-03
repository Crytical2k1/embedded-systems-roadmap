#include "sensor_task.h"
#include "sensor_driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

//forward declaration
static void sensor_data_task(void *pvParameters);

//task initialization
void sensor_task_init(QueueHandle_t queue) {
    xTaskCreate(sensor_data_task, "SensorDataTask", 2048, (void *)queue, 2, NULL);
}
//sensor task logic
static void sensor_data_task(void *pvParameters) {
    QueueHandle_t queue = (QueueHandle_t)pvParameters;
    int16_t SendValue[3];
    BaseType_t xStatus;

    while (1) {
        //read sensor and store values in buffer
        imu_read(SendValue);
        //send data to the queue
        xStatus = xQueueSendToBack(queue, (void *)SendValue, pdTICKS_TO_MS(10));
        //added extra time (10ms) in case the queue is full for a small time
        if (xStatus != pdPASS) {
            printf("Error: couldnt send the data to queue\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}