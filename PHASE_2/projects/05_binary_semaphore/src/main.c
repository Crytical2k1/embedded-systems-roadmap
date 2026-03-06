#include "button_task.h"
#include "button_driver.h"
#include "imu_driver.h"
#include "imu_task.h"
#include "lcd_manager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define IMU_TASK_PRIORITY 5U
#define BUTTON_TASK_PRIORITY 5U


void app_main() {
    button_init();
    imu_init();
    lcd_manager_init();    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    lcd_manager_write(0,0, "Sensor ready");
    vTaskDelay(pdMS_TO_TICKS(500));
    lcd_manager_clear();

    imu_task_init(IMU_TASK_PRIORITY);
    button_task_init(BUTTON_TASK_PRIORITY);
}