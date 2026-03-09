#include "buzzer_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


static const char *TAG = "buzzer_driver";

void buzzer_init(void) {
    gpio_config_t buzzer_config = {
        .pin_bit_mask = (1UL << buzzer_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    esp_err_t esp_ret = gpio_config(&buzzer_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize buzzer");
        abort();
    }
}

void buzzer_sound(void) {
    gpio_set_level(buzzer_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(buzzer_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(buzzer_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(buzzer_pin, 0);
}