#include "imu_driver.h"
#include "imu_task.h"
#include "counter_task.h"
#include "lcd_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main() {
    vTaskDelay(pdMS_TO_TICKS(200));
    imu_init();
    lcd_manager_init();    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    lcd_manager_write(0,0, "Sensor ready");
    vTaskDelay(pdMS_TO_TICKS(500));
    lcd_manager_clear();

    imu_task_init();
    counter_task_init();
}