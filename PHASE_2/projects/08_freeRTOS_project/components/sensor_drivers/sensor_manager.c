#include "sensor_manager.h"
#include "sensor_drivers.h"
#include "system_events.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"

static const char *TAG = "sensor_manager";
#define NTC_INIT_PRIO 5
#define PHOTO_INIT_PRIO 5
#define NTC_READ_PRIO 5
#define PHOTO_READ_PRIO 5

//forward declaration
static void ntc_manager_init(void *pvParameters);
static void photo_manager_init(void *pvParameters);
static void ntc_read_task(void *pvParameters);
static void photo_read_task(void *pvParameters);

//main manager function
void sensor_manager_init(void *pvParameters) {
    //TODO------------------------------------------------>
    //buzzer_init();

    //Create initialization tasks
    xTaskCreate(ntc_manager_init, "NTC_init", 2048, NULL, NTC_INIT_PRIO, NULL);
    xTaskCreate(photo_manager_init, "Photo_init", 2048, NULL, PHOTO_INIT_PRIO, NULL);
    ESP_LOGI(TAG, "Peripherals initialization, waiting to complete...");
    //Wait for init tasks to finish
    EventBits_t bits_recieved = xEventGroupWaitBits(
        system_event_group,
        ALL_INIT_DONE,
        pdTRUE,
        pdTRUE,
        pdMS_TO_TICKS(500)
    );
    //check event bits
    if((bits_recieved & ALL_INIT_DONE) != ALL_INIT_DONE) {
        ESP_LOGE(TAG, "Init timout, couldnt initialize all the periferals");
        if(!(bits_recieved & EVENT_NTC_INIT)) {
            ESP_LOGE(TAG, "NTC failed to initialize");
        }
        if(!(bits_recieved & EVENT_PHOTO_INIT)) {
            ESP_LOGE(TAG, "Photoresistor failed to initialize");
        }
        //As the initialization failed, we cant continue
        vTaskDelete(NULL);
        return;
    }
    //Initialization was succesful
    ESP_LOGI(TAG, "Peripherals init successfully");
    //TODO------------------------------------------------>
    //buzzer_sound();

    //set bits to system ready
    xEventGroupSetBits(system_event_group, EVENT_SYSTEM_READY);
    ESP_LOGI(TAG, "Event System Ready set. Running tasks...");
    //start running tasks
    xTaskCreate(ntc_read_task,"read_ntc", 4092, NULL, NTC_READ_PRIO, NULL);
    xTaskCreate(photo_read_task,"read_light", 4092, NULL, PHOTO_READ_PRIO, NULL);
    ESP_LOGI(TAG, "System startup complete. Loop running.");

    //manager init task done
    vTaskDelete(NULL);
}

static void ntc_manager_init(void *pvParameters) {
    ESP_LOGI(TAG, "NTC entered initialization");
    if(ntc_init() != ESP_OK) {
        ESP_LOGE(TAG, "NTC init failed");
    }
    ESP_LOGI(TAG, "NTC initialized successfully");
    xEventGroupSetBits(system_event_group, EVENT_NTC_INIT);
    vTaskDelete(NULL);
}

static void photo_manager_init(void *pvParameters) {
    ESP_LOGI(TAG, "Photoresistor entered initialization");
    if (photo_init() != ESP_OK) {
        ESP_LOGE(TAG, "Photoresistor init failed");
    }
    ESP_LOGI(TAG, "Photoresistor initialized successfully");
    xEventGroupSetBits(system_event_group, EVENT_PHOTO_INIT);
    vTaskDelete(NULL);
}

void ntc_read_task(void *pvParameters) {
    uint16_t raw_value;
    while (1) {
        raw_value = ntc_read();
        printf("Temp: %d\r\n", raw_value);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void photo_read_task(void *pvParameters) {
    uint16_t raw_value;
    while (1) {
        raw_value = photo_read();
        printf("Light: %d\r\n", raw_value);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}