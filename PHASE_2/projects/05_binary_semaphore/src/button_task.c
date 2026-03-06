#include "button_task.h"
#include "button_driver.h"
#include "lcd_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "button_task";

static void button_task(void *pvParameters);

void button_task_init(uint8_t prio) {
    BaseType_t task_created = xTaskCreate(button_task, "ButtonTask", 4096, NULL, prio, NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create button task");
        abort();
    }
}

static void button_task(void *pvParameters) {
    uint32_t count = 0;
    char line[32];

    //get the semaphore
    SemaphoreHandle_t button_semaphore = button_get_semaphore();
    
    while(1) {
        if (xSemaphoreTake(button_semaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
            ESP_LOGI(TAG, "Button pressed detected");
            snprintf(line, sizeof(line), "Count: %lu", ++count);
            lcd_manager_write(0,1, line);
        }
    }
    vTaskDelete(NULL);
}