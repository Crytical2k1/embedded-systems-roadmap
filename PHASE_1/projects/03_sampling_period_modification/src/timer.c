#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

static volatile uint32_t system_counter = 0;

void timer_init(void) {
    // stop timer
    TCCR1B = 0;
    //set timer mode
    TCCR1A = 0;
    TCCR1B |= (1 << WGM12);
    //set compare value
    OCR1A = 249; //corresponding value to prescaler = 64
    //set interrupt 
    TIMSK1 |= (1 << OCIE1A);
    //start timer (prescaler 64)
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

ISR(TIMER1_COMPA_vect){
    system_counter++ ;
}

uint32_t get_time_ms(void){
    uint32_t time;
    //stop interrupts
    cli();
    //copy value
    time = system_counter;
    //start interrupts
    sei();

    return time;
}