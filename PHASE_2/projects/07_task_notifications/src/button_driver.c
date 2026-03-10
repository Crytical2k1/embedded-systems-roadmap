#include "button_driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "button_driver";
static TaskHandle_t button_task_handle = NULL;
static volatile uint64_t last_press_us = 0;

void button_driver_set_task_handle(TaskHandle_t handle) {
    button_task_handle = handle;
}
//IRAM_ATTR forces function into internal ram so it executes even when flash cache is disabled.
//All ISR funct and any they call must carry IRAM_ATTR in ESP32
static void IRAM_ATTR button_isr_handler(void *args) {
    //function to minimize the action of contact bounce of the button (multiple activations in a short period of time due to pin contacts)
    //checks the time since the last press and if is less than the debounce const it ignores the press.
    const uint64_t now_us = esp_timer_get_time();
    if ((now_us - last_press_us) < (BUTTON_DEBOUNCE_MS * 1000LL)) {
        return;
    }
    last_press_us = now_us;

    //Give semaphore
    BaseType_t higher_priority_task_woken = pdFALSE;
    if (button_task_handle != NULL) {
        vTaskNotifyGiveFromISR(button_task_handle, &higher_priority_task_woken);
    }
    //context switch
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

void button_init(void) {
    //1. configure the gpio pin
    const gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << GPIO_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE, // falling edge interrupt
    };
    esp_err_t esp_ret = gpio_config(&button_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_config failed: %s", esp_err_to_name(esp_ret));
    }
    //2. install the gpio isr service
    esp_ret = gpio_install_isr_service(0);
    if (esp_ret != ESP_OK && esp_ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "gpio_install_isr_service failed %s", esp_err_to_name(esp_ret));
    }
    //3. register the pin isr handler
    esp_ret = gpio_isr_handler_add(GPIO_BUTTON, button_isr_handler, NULL);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_isr_handler_add failed %s", esp_err_to_name(esp_ret));
    }

    //if everything worked well, log successful init.
    ESP_LOGI(TAG, "Initialised. GPIO %d, debounce %u ms", GPIO_BUTTON, BUTTON_DEBOUNCE_MS);
}

