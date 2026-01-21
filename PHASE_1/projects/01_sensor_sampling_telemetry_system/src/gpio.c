#include "gpio.h"
#include <avr/io.h>

#define LED_PORT PORTB //controls output value
#define LED_DDR DDRB //controls if the port is output/input
#define LED_BIT 5

void gpio_init(void) {
    //set led port as output
    LED_DDR |= (1 << LED_BIT);
}

void gpio_toggle_led(void) {
    //toggle bit
    LED_PORT ^= (1 << LED_BIT);
}