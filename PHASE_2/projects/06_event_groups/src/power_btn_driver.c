#include "power_btn_driver.h"
#include "system_events.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"

static const char *TAG = "power button driver";
static SemaphoreHandle_t power_btn_semaphore = NULL;

static void IRAM_ATTR power_btn_isr_handler(void *args) {
    //Give Semaphore
    BaseType_t higher_priority_task_woken = pdFALSE;
    xSemaphoreGiveFromISR(power_btn_semaphore, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void power_btn_task(void *pvParameters) {
    while (1) {
        if(xSemaphoreTake(power_btn_semaphore, portMAX_DELAY) == pdTRUE) {
            //wait for button bounc
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_MS));
            //check again the button state
            if (gpio_get_level(pwr_btn_pin) == 0) {
                //button was pressed
                xEventGroupSetBits(system_event_group, EVENT_PWR_BTN_PRESSED);
                vTaskDelete(NULL);
            } else {
                ESP_LOGE(TAG, "Debounce rejected, state high after debounce");
            }
        }
    }
}

esp_err_t power_btn_init(uint8_t priority) {
    //create binary semaphore
    power_btn_semaphore = xSemaphoreCreateBinary();
    if (power_btn_semaphore == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        return ESP_FAIL;
    }

    //configure gpio button
    gpio_config_t power_btn_config = {
        .pin_bit_mask = (1ULL << pwr_btn_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    esp_err_t esp_ret = gpio_config(&power_btn_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize power button gpio");
        vSemaphoreDelete(power_btn_semaphore);
        power_btn_semaphore = NULL;
        return ESP_FAIL;
    }
    //install gpio isr service
    esp_ret = gpio_install_isr_service(0);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install isr service");
        vSemaphoreDelete(power_btn_semaphore);
        power_btn_semaphore = NULL;
        return ESP_FAIL;
    }
    //register pin isr handler
    esp_ret = gpio_isr_handler_add(pwr_btn_pin, power_btn_isr_handler, NULL);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register isr");
        vSemaphoreDelete(power_btn_semaphore);
        power_btn_semaphore = NULL;
        return ESP_FAIL;
    }
    //create debounce task
    esp_ret = xTaskCreate(
        power_btn_task,
        "power_button_task",
        2048,
        NULL,
        priority,
        NULL
    );
    if (esp_ret != pdPASS) {
        ESP_LOGE(TAG, "FATAL, failed to create power button task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Power button initialized successfully");
    return ESP_OK;
}