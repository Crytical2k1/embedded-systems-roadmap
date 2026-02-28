#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"

void led_task(void *pvParameters) {
    //configure GPIO
    led_init();

    //blinking loop
    while (1) {
        led_toggle(); //toggle led
        vTaskDelay(pdMS_TO_TICKS(200)); //500ms delay
    }
}