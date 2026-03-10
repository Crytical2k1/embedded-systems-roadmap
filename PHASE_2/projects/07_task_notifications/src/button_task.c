#include "button_task.h"
#include "button_driver.h"
#include "lcd_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "button_task";
static TaskHandle_t button_task_handle = NULL;

static void button_task(void *pvParameters);

void button_task_init(uint8_t prio) {
    BaseType_t task_created = xTaskCreate(button_task, "ButtonTask", 4096, NULL, prio, &button_task_handle);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create button task");
        abort();
    }
    button_driver_set_task_handle(button_task_handle);
}

static void button_task(void *pvParameters) {
    uint32_t count = 0;
    char line[32];
    
    while(1) {
        if (ulTaskNotifyTake( pdTRUE, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Button pressed detected");
            snprintf(line, sizeof(line), "Count: %lu", ++count);
            lcd_manager_write(0,1, line);
        }
    }
    vTaskDelete(NULL);
}