#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

void led_init(void);
void shift595(uint8_t data);
uint8_t build_led_state(float roll, float pitch);


#endif