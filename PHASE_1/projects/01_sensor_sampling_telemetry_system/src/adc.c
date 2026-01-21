#include <avr/io.h>
#include "adc.h"

void adc_init(void) {
    ADMUX = (1 << REFS0); //set voltage reference to AVcc
    // enable ADC and set prescaler (128)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_sample(uint8_t channel) {
    //select channel, clean old channel bits, keep reference bits
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    //start conversion
    ADCSRA |= (1 << ADSC);
    //wait for conversion
    while (ADCSRA & (1 << ADSC)) {
        //busy wait
    }
    //read the result
    uint16_t result = ADCL;
    result |= (ADCH << 8);
    return result;
}