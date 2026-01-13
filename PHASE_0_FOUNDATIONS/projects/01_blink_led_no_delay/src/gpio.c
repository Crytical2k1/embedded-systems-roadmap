#include "gpio.h"
#include <avr/io.h> //MCU-specific header for registers

//port and bit assignation
#define LED_PORT PORTB // controls output value
#define LED_DDR DDRB // controls the direction of the the pin (in/out)
#define LED_BIT 5 // selects the corresponding pin

void gpio_init(void){
    //set DDR bit to work as output (1)
    LED_DDR |= (1 << LED_BIT); //Bitwise set
};

void gpio_toggle_led(void){
    //toggle bit
    LED_PORT ^= (1 << LED_BIT);
};