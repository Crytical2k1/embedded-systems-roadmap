#include "lcd_manager.h"
#include "lcd_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t xMutex;

void lcd_manager_init(void) {
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        printf("Mutex, failed to create LCD mutex");
        //modify with system reset
        while(1) { vTaskDelay(portMAX_DELAY);}
    }
    lcd_init();
}

void lcd_manager_write(uint8_t cols, uint8_t rows, const char *str) {
    if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        lcd_set_cursor(cols, rows);
        lcd_write_str(str);
        xSemaphoreGive(xMutex);
    } else {
        printf("Mutex, lcd timeout from task");
    }
}

void lcd_manager_clear(void) {
    if(xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        lcd_clear();
        xSemaphoreGive(xMutex);
    } else {
        printf("Mutex, lcd timeout from task");
    }
}