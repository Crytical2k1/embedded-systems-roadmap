#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "timer.h"
#include "isr_flags.h" //handles the isr flag

//PORT definition for the output led and input button
#define LED_PORT PORTB
#define LED_DDR DDRB
#define LED_BIT 5
//PD2 as the external interrupt 0 (INT0) is hardwired to pin PD2
#define BUTTON_PORT PORTD
#define BUTTON_DDR DDRD
#define BUTTON_PIN PIND
#define BUTTON_BIT 2

//Debounce definition (prevent multime flag firing when pressing the button for long time)
#define DEBOUNCE_TIME 50 //50ms

void gpio_init(void) {
    //set led as output
    LED_DDR |= (1 << LED_BIT); 
    //set button as input
    BUTTON_DDR &= ~(1 << BUTTON_BIT); 
    //enable pull-up resistor on button
    BUTTON_PORT |= (1 << BUTTON_BIT);

    //configure the external interrupt (INT0)
    //set INT0 to trigger on falling edge (button press)
    EICRA |= (1 <<ISC01);
    EICRA &= ~(1 << ISC00);
    //enable INT0
    EIMSK |= (1 << INT0);
    //enable global interrupts
    sei();
}

ISR(INT0_vect) {
    uint32_t now = timer_get_ms();
    if ((now - last_interrupt_time) > DEBOUNCE_TIME) {
        isr_event_flag = 1;
        last_interrupt_time = now;
    }
}

void gpio_toggle_led(void) {
    LED_PORT ^= (1 << LED_BIT); //toggle the LED bit
}