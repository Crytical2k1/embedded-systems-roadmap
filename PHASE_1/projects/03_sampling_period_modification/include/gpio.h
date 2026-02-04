#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>

void gpio_init(void);
void gpio_toggle_led(void);
void gpio_sample_toggle_led(uint8_t i);

#endif