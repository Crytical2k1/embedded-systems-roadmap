#include "sensor_manager.h"
#include "system_events.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "main";

#define SENSOR_MANAGER_PRIO 6

void app_main() {
    vTaskDelay(pdMS_TO_TICKS(2000));
    //create event group
    system_event_group = xEventGroupCreate();
    if (system_event_group == NULL) {
        ESP_LOGE(TAG, "Failed to create event group");
        while(1) {vTaskDelay(portMAX_DELAY);}
    }
    ESP_LOGI(TAG, "Event group created");
    //initialize sensor manager
    esp_err_t esp_ret = xTaskCreate(sensor_manager_init, "init_sensors", 2048, NULL, SENSOR_MANAGER_PRIO, NULL);
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create sensor manager task");
        return;
    }
    ESP_LOGI(TAG, "Successfully create sensor manager.");
}