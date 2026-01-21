#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

static volatile uint32_t system_time_ms = 0;

void timer_init(void) {
    //stop timer
    TCCR1B = 0;
    //set mode (CTC)
    TCCR1A = 0;
    TCCR1B |= (1 << WGM12);
    // set compare value
    OCR1A = 249;
    //enable interrupt
    TIMSK1 |= (1 << OCIE1A);
    //start timer
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

ISR(TIMER1_COMPA_vect) {
    system_time_ms++;
}

uint32_t timer_get_ms(void) {
    uint32_t time;
    //stop interrupts
    cli();
    //copy value
    time = system_time_ms;
    //enable interrupts
    sei();

    return time;
}