#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//time variable only accessed from timer.c, 
//modified only in ISR and read via timer_get_ms()
static volatile uint32_t system_time_ms = 0;

void timer_itit(void) {
    //stop the timer
    TCCR1B = 0;
    //select timer mode (CTC)
    TCCR1A = 0;
    TCCR1B |= (1 << WGM12); // set to 1 just WGM12, rest to 0
    //set compare value
    //calculate prescaler value
    // MCU clock = 16MHz / prescaler = 64
    // 16.000.000 / 64 = 250.000 MHz -> 1ms
    // Ticks per interrupt -> 250.000 x 0.001 = 250
    OCR1A = 249; // as count starts at 0
    //enable interrupt
    TIMSK1 |= (1 << OCIE1A);
    //start timer
    //prescaler 64 corresponds to: CS11 and CS10 1
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

//
ISR(TIMER1_COMPA_vect) {
    //keeping it simple read-modify-write
    system_time_ms++;
}

uint32_t timer_get_ms(void) {
    //as the time is a 32bit variable and the MCU is 8-bit,
    //it will take 4 CPU instructions to copy the value
    //this is not atomic and we need to prevent interrupts from firing
    uint32_t time;

    cli(); //enter critical section
    time = system_time_ms;
    sei(); //exit critical section

    return time;
}