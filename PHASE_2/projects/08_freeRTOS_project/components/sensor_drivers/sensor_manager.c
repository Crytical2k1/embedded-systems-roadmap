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

#define NTC_SENSOR_ID 0
#define PHOTO_SENSOR_ID 1

// sensor sampling period
#define NTC_PERIOD 1000
#define PHOTO_PERIOD  500

//forward declaration
static void ntc_manager_init(void *pvParameters);
static void photo_manager_init(void *pvParameters);
static void ntc_read_timer(TimerHandle_t xTimer);
static void photo_read_timer(TimerHandle_t xTimer);

//main manager function
void sensor_manager_init(void *pvParameters) {
    QueueHandle_t raw_data_queue = pvParameters;
    TimerHandle_t xTimerNTC, xTimerPHOTO;
    
    //TODO------------------------------------------------>
    //buzzer_init();

    //set ADC bit width
    esp_err_t esp_ret = adc1_config_width(ADC_WIDTH_BIT_12);
    if(esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set ADC bit width");
    }
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
    //start running timers
    xTimerNTC = xTimerCreate("NTC_timer", pdMS_TO_TICKS(NTC_PERIOD), pdTRUE, (void *)raw_data_queue, ntc_read_timer);
    xTimerPHOTO = xTimerCreate("PHOTO_timer", pdMS_TO_TICKS(PHOTO_PERIOD), pdTRUE, (void *)raw_data_queue, photo_read_timer);
    if ((xTimerNTC == NULL) || (xTimerPHOTO == NULL)) {
        ESP_LOGE(TAG, "Failed to create sensor timers");
    } else {
        if ((xTimerStart(xTimerNTC, 0) != pdPASS) || (xTimerStart(xTimerPHOTO, 0) != pdPASS)) {
            ESP_LOGE(TAG, "Failed to start sensor timers");
        }
    }
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

static void ntc_read_timer(TimerHandle_t xTimer) {
    QueueHandle_t raw_data_queue = (QueueHandle_t) pvTimerGetTimerID(xTimer);
    raw_data_t raw_data_ntc;
    raw_data_ntc.sensor_id = NTC_SENSOR_ID;
    raw_data_ntc.data = ntc_read();
    xQueueSendToBack(raw_data_queue, &raw_data_ntc, 0);
}
static void photo_read_timer(TimerHandle_t xTimer) {
    QueueHandle_t raw_data_queue = (QueueHandle_t) pvTimerGetTimerID(xTimer);
    raw_data_t raw_data_photo;
    raw_data_photo.sensor_id = PHOTO_SENSOR_ID;
    raw_data_photo.data = photo_read();
    xQueueSendToBack(raw_data_queue, &raw_data_photo, 0);
}