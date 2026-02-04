#include "gpio.h"
#include "isr_flags.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//sampling rate led
#define LED_PORT PORTB
#define LED_DDR DDRB
#define LED_BIT 5 
//sampling rate button (digital pin 2 INT0)
#define BUTTON_PORT PORTD
#define BUTTON_DDR DDRD
#define BUTTON_BIT 2 
//sampling toggle leds
#define LED_TOGGLE_PORT PORTD
#define LED_TOGGLE_DDR DDRD
#define LED_TOGGLE_BIT_FAST 4
#define LED_TOGGLE_BIT_MEDIUM 5
#define LED_TOGGLE_BIT_SLOW 6
static const uint8_t led_list[3] = {LED_TOGGLE_BIT_FAST , LED_TOGGLE_BIT_MEDIUM, LED_TOGGLE_BIT_SLOW};


void gpio_init(void) {
    //set pin direction
    LED_DDR |= (1 << LED_BIT); // output
    BUTTON_DDR &= ~(1 << BUTTON_BIT); //input
    LED_TOGGLE_DDR |= (1 << LED_TOGGLE_BIT_FAST) | (1 << LED_TOGGLE_BIT_MEDIUM) | (1 << LED_TOGGLE_BIT_SLOW);
    //set pull up resistor to button
    BUTTON_PORT |= (1 << BUTTON_BIT);

    //set up interrupt INT0
    //configure to trigger on falling edge
    EICRA &= ~(1 << ISC00);
    EICRA |= (1 << ISC01);
    //enable INT0
    EIMSK |= (1 << INT0);
}

ISR(INT0_vect) {
    isr_sampling_button_flag = 1;
}
void gpio_toggle_led(void) {
    //set output value
    LED_PORT ^= (1 << LED_BIT);
}
void gpio_sample_toggle_led(uint8_t i) {
    LED_TOGGLE_PORT &= ~(1 << led_list[(i + 1) % 3]);
    LED_TOGGLE_PORT &= ~(1 << led_list[(i + 2) % 3]);
    LED_TOGGLE_PORT |= (1 << led_list[i % 3]);
}