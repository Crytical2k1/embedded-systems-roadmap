#include "lcd_manager.h"
#include "lcd_driver.h"
#include "system_events.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static SemaphoreHandle_t xMutex;
static const char *TAG = "lcd manager";

void lcd_manager_init(void *pvParameters){
    ESP_LOGI(TAG, "LCD started initizalization");
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex");
        while(1) {vTaskDelay(portMAX_DELAY);}
    }

    if (lcd_init() != ESP_OK) {
        ESP_LOGE(TAG, "LCD init failed, System cannot proceed safely");
    }
    //set init task
    xEventGroupSetBits(system_event_group, EVENT_LCD_INIT_DONE);
    ESP_LOGI(TAG, "LCD initialized successfully");
    vTaskDelete(NULL);
}
void lcd_manager_write(uint8_t col, uint8_t row, const char *str){
    //if the mutex is available
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        lcd_set_cursor(col, row);
        lcd_write_str(str);
        //once the task is done, give mutex back
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Mutex unavailable");
    }
}
void lcd_manager_clear(void) {
    //if the mutex is available
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        lcd_clear();
        //once the task is done, give mutex back
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Mutex unavailable");
    }
}