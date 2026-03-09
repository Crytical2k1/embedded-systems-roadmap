#include "imu_driver.h"
#include "buzzer_driver.h"
#include "lcd_manager.h"
#include "imu_task.h"
#include "system_events.h"
#include "power_btn_driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define imu_prio 5U
static const char *TAG = "Main";

#define IMU_TASK_INIT_PRIO 5
#define LCD_TASK_INIT_PRIO 5
#define IMU_TASK_PRIO 5
#define MANAGER_TASK_PRIO 6
#define POWER_BTN_PRIO 10

static void start_manager_task(void *pvParameters) {
    ESP_LOGI(TAG, "Waiting for power button to be pressed...");
    //wait for power button to be pressed
    xEventGroupWaitBits(
        system_event_group,
        EVENT_PWR_BTN_PRESSED,
        pdTRUE,
        pdTRUE,
        portMAX_DELAY
    );
    ESP_LOGI(TAG, "Power button received. Starting system...");
    //initialize the buzzer
    buzzer_init();
    //initialize sensors and components
    xTaskCreate(imu_task_init, "imu_init", 4096, NULL, IMU_TASK_INIT_PRIO, NULL);
    xTaskCreate(lcd_manager_init, "lcd_init", 4096, NULL, LCD_TASK_INIT_PRIO, NULL);
    ESP_LOGI(TAG, "Peripherals initialization, waiting to complete...");
    //wait for tasks to initialize
    EventBits_t bits_recieved = xEventGroupWaitBits(
        system_event_group,
        ALL_INITS_DONE,
        pdTRUE,
        pdTRUE,
        pdMS_TO_TICKS(10000)
    );
    //check all inits completed
    if ((bits_recieved & ALL_INITS_DONE) != ALL_INITS_DONE) {
        ESP_LOGE(TAG, "Init timeout. bits_recieved 0x%02X, expected 0x%02X", (unsigned)bits_recieved, (unsigned)ALL_INITS_DONE);
        if (!(bits_recieved & EVENT_IMU_INIT_DONE)) {
            ESP_LOGE(TAG, "--> IMU failed to initialize");
        }
        if (!(bits_recieved & EVENT_LCD_INIT_DONE)) {
            ESP_LOGE(TAG, "--> LCD failed to initialize");
        }
        vTaskDelete(NULL);
        return;
    }
    //once initialized make a sound
    ESP_LOGI(TAG, "Peripherals successfully initialized");
    ESP_LOGI(TAG, "Startup confirmation beep");
    buzzer_sound();
    //set system as ready
    xEventGroupSetBits(system_event_group, EVENT_SYSTEM_READY);
    ESP_LOGI(TAG, "EVENT_SYSTEM_READY set. Running IMU task");
    //initialize system tasks
    xTaskCreate(imu_task, "imu_run",4096, NULL, IMU_TASK_PRIO, NULL);
    ESP_LOGI(TAG, "System startup complete. Loop running.");

    //delete to free stack memory
    vTaskDelete(NULL);
}

void app_main() {
    //create event group
    system_event_group = xEventGroupCreate();
    if (system_event_group == NULL) {
        ESP_LOGE(TAG, "Failed to create event group");
        while(1) {vTaskDelay(portMAX_DELAY);}
    }
    ESP_LOGI(TAG, "System event group created");
    //init power button before the manager task
    if(power_btn_init(POWER_BTN_PRIO) != ESP_OK) {
        ESP_LOGE(TAG, "FATAL, power button init failed.");
        return;
    }
    //create manager task
    esp_err_t esp_ret = xTaskCreate(
        start_manager_task,
        "Startup_manager",
        4096,
        NULL,
        MANAGER_TASK_PRIO, 
        NULL
    );
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create startup manager task");
        return;
    }
    ESP_LOGI(TAG, "Startup manager task created. Press power button to begin.");

    
}