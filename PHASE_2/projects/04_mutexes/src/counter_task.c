#include "counter_task.h"
#include "lcd_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//forward declaration
static void counter_task(void *pvParameters);

void counter_task_init(void) {
    xTaskCreate(counter_task, "CounterTask", 2048, NULL, 5, NULL);
}

static void counter_task(void *pvParameters) {
    uint32_t count = 0;
    char line[32];

    while (1) {
        snprintf(line, sizeof(line), "Count: %lu" ,count++);
        lcd_manager_write(0,1, line);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}