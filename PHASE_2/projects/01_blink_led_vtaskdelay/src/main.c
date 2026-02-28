#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_GPIO GPIO_NUM_2 //built in led

void led_task(void *pvParameters);

void app_main(void) {
  xTaskCreate(
    led_task,         //Task function
    "LED Task",       //Task name
    2048,             //Stack size (bytes)
    NULL,             //Parameters
    5,                //Priority (ESP32 0-25)
    NULL              //Task handle
  );
}