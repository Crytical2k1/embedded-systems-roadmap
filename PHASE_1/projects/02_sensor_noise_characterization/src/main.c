#include <avr/interrupt.h>
#include "timer.h"
#include "adc.h"
#include "serial.h"
#include "gpio.h"
#include "app.h"

// Configuration
#define BAUDRATE 9600

int main(void) {
    gpio_init();
    timer_init();
    serial_init(BAUDRATE);
    adc_init();
    sei();

    serial_write_string("System ready\n");

    while(1){
        handle_serial_rx();
        app_run_sampling();
    }
}