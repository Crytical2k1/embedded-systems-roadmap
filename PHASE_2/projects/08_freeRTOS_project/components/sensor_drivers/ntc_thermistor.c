#include "include\sensor_drivers.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "ntc_driver";
static const uint8_t ntc_pin = ADC1_CHANNEL_7;

esp_err_t ntc_init(void) {
    esp_err_t esp_ret = adc1_config_channel_atten(ntc_pin, ADC_ATTEN_DB_12); 
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ntc gpio");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "NTC sensor connected");
    return ESP_OK;
}

uint16_t ntc_read(void) {
    uint16_t value = adc1_get_raw(ntc_pin);
    return value;
}