#include "adc.h"
#include <avr/io.h>

void adc_init(void) {
    //set voltage reference
    ADMUX = (1 << REFS0);
    //control and status register
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    //aden enables the conversor
    //ADPS sets the prescaler (to 128)
}

uint16_t adc_sample(uint8_t channel) {
    //select channel, clean old channel bits and keep reference bits
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    //start conversion
    ADCSRA |= (1 << ADSC);
    //wait for conversion to finish
    while (ADCSRA & (1 << ADSC)) {
        //busy wait
    }
    //read value (first low, then high)
    uint16_t result = ADCL;
    result |= (ADCH << 8);
    return result;
}