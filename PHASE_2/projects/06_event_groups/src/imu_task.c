#include "imu_task.h"
#include "imu_driver.h"
#include "lcd_manager.h"
#include "system_events.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static const char *TAG = "IMU task";

void imu_task(void *pvParameters){
    int16_t accel[3];
    char line[32];
    while (1) {
        imu_read(accel);
        
        sprintf(line, "%d x, %d y, %d z", accel[0], accel[1], accel[2]);
        lcd_manager_write(0,0, line);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    
}
void imu_task_init(void *pvParameters) {
    ESP_LOGI(TAG, "IMU entered initizalization");
    if (imu_init() != ESP_OK) {
        ESP_LOGE(TAG, "IMU init Failed! System cannot proceed safely.");
    }
    //set init task
    xEventGroupSetBits(system_event_group, EVENT_IMU_INIT_DONE);
    ESP_LOGI(TAG, "IMU initialized succesfully");
    
    vTaskDelete(NULL);
}