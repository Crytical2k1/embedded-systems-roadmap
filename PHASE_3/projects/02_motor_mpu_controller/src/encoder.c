#include "encoder.h"

#include "driver/gpio.h"
#include "esp_intr_alloc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define pulses_per_rev 390
#define sampling_time_ms 100
#define sampling_time_s (sampling_time_ms / 1000.0f)


static volatile int32_t encoder_count = 0;
static int32_t previous_count = 0;
static float rpm = 0.0f;

static void encoder_isr(void *arg) {
    encoder_count++;
}

static void encoder_task(void *pvParameters) {

    while (1) {
        int32_t current = encoder_count;
        int32_t delta = current - previous_count;
        previous_count = current;

        rpm = delta * 60.0f / (pulses_per_rev * sampling_time_s);

        vTaskDelay(pdMS_TO_TICKS(sampling_time_ms));
    }
}

void encoder_init(void) {
    gpio_config_t io_config = {
        .pin_bit_mask = 
        (1ULL << GPIO_HALL1)|
        (1ULL << GPIO_HALL2),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&io_config);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(
        GPIO_HALL1,
        encoder_isr,
        NULL);

    xTaskCreate(
        encoder_task,
        "Encoder_Task",
        2048,
        NULL,
        5,
        NULL
    );
}

int32_t encoder_get_count(void) {
    return encoder_count;
}

void encoder_reset(void) {
    encoder_count = 0;
}

float encoder_get_rpm(void) {
    return rpm;
}