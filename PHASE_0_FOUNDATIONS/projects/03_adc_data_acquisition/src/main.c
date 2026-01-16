#include "adc.h"
#include "serial.h"
#include "timer.h"

#include <avr/interrupt.h>

#define BAUD_RATE 9600 
#define SAMPLE_PERIOD 1000 //100ms
#define ADC_CHANNEL 1 
#define REF_VOLT 5000UL //5V - 5000mV

int main(void) {
    timer_init();
    adc_init();
    serial_init(BAUD_RATE);
    sei(); // enable global interrupts

    static uint32_t last_sample_time = 0;

    while(1) {
        uint32_t now = timer_get_ms();

        if ((now - last_sample_time) >= SAMPLE_PERIOD) {
            //get the info
            uint16_t adc_value = adc_sample(ADC_CHANNEL);
            uint16_t value_mv = (adc_value * REF_VOLT) / 1024; //UL states an unsigned long value
            //send the info
            serial_write_uint16(value_mv / 1000);  // whole volts
            serial_write_char('.');
            serial_write_uint16((value_mv % 1000) / 100); // first decimal
            serial_write_char('V');
            serial_write_char('\r');
            serial_write_char('\n');

            last_sample_time = now;
        }
    }
}