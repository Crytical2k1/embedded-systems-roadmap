#include "data_processor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "esp_log.h"

static const char *TAG = "data_status_task";

#define DATA_PIN 23 //DP
#define LATCH_PIN 5 // ST_CP
#define CLOCK_PIN 18 // SH_CP

//forward declaration
static void send_data_status(uint8_t data_status);

void data_status_task(void *pvParameters) {

    //setup 74HC595
    gpio_config_t dev_config = {
        .pin_bit_mask = (1ULL << DATA_PIN) | (1ULL << LATCH_PIN) | (1ULL << CLOCK_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    esp_err_t esp_ret = gpio_config(&dev_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set up 74HC595 gpio");
        abort();
    }

    while (1) {
        //receive data from task notification
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        send_data_status(latest_data_status);
    }
}

static void send_data_status(uint8_t data_status) {
    //set to low to start loading bits
    gpio_set_level(LATCH_PIN, 0);
    for (uint8_t i = 0; i < 8; i++) {
        //set each bit, on each clock cicle
        gpio_set_level(CLOCK_PIN, 0);
        gpio_set_level(DATA_PIN, data_status >> i & 1);
        gpio_set_level(CLOCK_PIN, 1);
    }
    //set to high to send the register
    gpio_set_level(LATCH_PIN, 1);
}