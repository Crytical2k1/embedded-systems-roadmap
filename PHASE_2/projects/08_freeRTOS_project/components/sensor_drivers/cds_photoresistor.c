#include "include\sensor_drivers.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "photo_driver";
static const uint8_t photo_pin = ADC1_CHANNEL_6;

esp_err_t photo_init(void) {
    esp_err_t esp_ret = adc1_config_channel_atten(photo_pin, ADC_ATTEN_DB_12); 
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure photoresistor gpio");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Light sensor connected");
    return ESP_OK;
}

uint16_t photo_read(void) {
    uint16_t value = adc1_get_raw(photo_pin);
    return value;
}