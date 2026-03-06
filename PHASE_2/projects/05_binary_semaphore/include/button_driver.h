#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include "freertos/FreeRTOS.h"

#define GPIO_BUTTON 19
#define BUTTON_DEBOUNCE_MS 50U

void button_init(void);
SemaphoreHandle_t button_get_semaphore(void);

#endif