#ifndef BUZZER_DRIVER_H
#define BUZZER_DRIVER_H

#include "driver/gpio.h"

//config
static const gpio_num_t buzzer_pin = GPIO_NUM_18;

void buzzer_init(void);
void buzzer_sound(void);

#endif