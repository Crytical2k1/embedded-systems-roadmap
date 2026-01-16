#include <avr/io.h>
#include "adc.h"

void adc_init(void) {
    //ADMUX - ADC multiplexer selection register
    ADMUX = (1 << REFS0); //sets voltage reference to AVcc(5V)

    //ADCSRA - ADC control and status register A
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    //ADEN enables the ADC
    //ADPS2:0 sets the prescaler for the clock in bits (2 1 0) -> (1 1 1) = 128
}

uint16_t adc_sample(uint8_t channel) {
    //Select ADC channel, clear old channel bits, keep reference bits
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    //Start conversion
    ADCSRA |= (1 << ADSC);
    //Wait for conversion to finish (ADSC becomes 0 when done)
    while (ADCSRA & (1 << ADSC)) {
        ; //busy wait 
        //usually bad, in this case CPU blocks for <1% of the sampling period so it is not relevant
        //instead you could use interrupts
    }
    //Read result (first low, then high)
    uint16_t result = ADCL;
    result |= (ADCH << 8); //move all the bits 8 spots
    //ADCL = 00100100
    //ADCH = 00100100
    //result = 00100100(ADCL) + 00100100 00000000 (ADCH+8-0) -> 16-bit

    return result;

}