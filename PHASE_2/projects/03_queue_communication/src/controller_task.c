#include "controller_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

//constants and helpers
#define GRAV_FACTOR 16384
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} device_orientation_t;
static device_orientation_t orientation;

//forward declaration
static void controller_task(void *pvParameters);

//task initialization
void controller_task_init(QueueHandle_t queue) {
    xTaskCreate(controller_task, "ControllerTask", 2048, (void *)queue, 1, NULL);
}

//controller task logic
static void controller_task( void *pvParameters ) {
    QueueHandle_t queue = pvParameters;
    int16_t RecievedValue[3];
    BaseType_t xStatus;

    while (1) {
        //read data from queue when available (portMAX_DELAY)
        xStatus = xQueueReceive( queue, RecievedValue, portMAX_DELAY);

        if (xStatus != pdPASS) {
            printf("Controller: queue receive error");
        } else {
            //handling the data recieved
            orientation.x = RecievedValue[0];
            orientation.y = RecievedValue[1];
            orientation.z = RecievedValue[2] - GRAV_FACTOR;
            printf("Acceleration: %d x, %d y, %d z\r\n", orientation.x, orientation.y, orientation.z);
            vTaskDelay(pdMS_TO_TICKS(250));
        }
    }
}