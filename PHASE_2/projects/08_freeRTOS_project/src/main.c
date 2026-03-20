#include "sensor_manager.h"
#include "system_events.h"
#include "data_processor.h"
#include "telemetry.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "main";

#define SENSOR_MANAGER_PRIO 6
#define PROCESS_DATA_PRIO 5
#define TELEMETRY_PRIO 2

void app_main() {
    vTaskDelay(pdMS_TO_TICKS(2000));
    //initialize wifi connection
    wifi_init_sta();
    //create event group
    system_event_group = xEventGroupCreate();
    if (system_event_group == NULL) {
        ESP_LOGE(TAG, "Failed to create event group");
        while(1) {vTaskDelay(portMAX_DELAY);}
    }
    ESP_LOGI(TAG, "Event group created");
    //create telemetry data queue
    QueueHandle_t telemetry_queue = xQueueCreate(6, sizeof(telemetry_format_t));
    if (telemetry_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create telemetry queue");
        while (1) {vTaskDelay(portMAX_DELAY);}
    }
    ESP_LOGI(TAG, "Telemetry queue created");
    //create raw_data queue
    QueueHandle_t raw_data_queue = xQueueCreate(6, sizeof(raw_data_t));
    if (raw_data_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create raw_data queue");
        while(1) {vTaskDelay(portMAX_DELAY);}
    }
    ESP_LOGI(TAG, "Raw_data queue created");
    //initialize telemetry task
    esp_err_t esp_ret = xTaskCreate(telemetry_send_task, "send_telemetry", 4096, (void *)telemetry_queue, TELEMETRY_PRIO, NULL);
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create telemetry task");
    }
    ESP_LOGI(TAG, "Successfully create data telemetry task.");
    //initialize processing task
    queue_params_t *params = malloc(sizeof(queue_params_t));
    params->raw_queue = raw_data_queue;
    params->telemetry_queue = telemetry_queue;
    esp_ret = xTaskCreate(raw_proccesing_task, "raw_processing_task", 4096, (void *)params, PROCESS_DATA_PRIO, NULL);
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create data processing task");
        return;
    }
    ESP_LOGI(TAG, "Successfully create data processing task.");

    //initialize sensor manager
    esp_ret = xTaskCreate(sensor_manager_init, "init_sensors", 2048, (void *)raw_data_queue, SENSOR_MANAGER_PRIO, NULL);
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create sensor manager task");
        return;
    }
    ESP_LOGI(TAG, "Successfully create sensor manager.");
}