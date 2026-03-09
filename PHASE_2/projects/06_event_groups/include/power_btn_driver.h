#ifndef POWER_BTN_DRIVER_H
#define POWER_BTN_DRIVER_H

#include "driver/gpio.h"

//config
static const gpio_num_t pwr_btn_pin = GPIO_NUM_5;
#define BUTTON_DEBOUNCE_MS 50

esp_err_t power_btn_init(uint8_t priority);

#endif