#include <avr/io.h>
#include "gpio.h"

#define ANALOG_PORT PORTC
#define ANALOG_DDR DDRC
#define ANALOG_PIN 1

void gpio_init(void) {
    //clear bits to set as input and disable pull-up 
    ANALOG_DDR &= ~(1 << ANALOG_PIN);
    ANALOG_PORT &= ~(1 << ANALOG_PIN);
}