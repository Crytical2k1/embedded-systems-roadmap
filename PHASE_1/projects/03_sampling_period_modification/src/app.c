#include "app.h"
#include "timer.h"
#include "serial.h"
#include "gpio.h"
#include "isr_flags.h"
#include "adc.h"
#include <stdint.h>
#include <avr/interrupt.h>

//constants
static const uint16_t sample_rate_list[3] = {100, 500, 1000};
static const uint8_t pressed_period = 200;
#define ADC_CHANNEL 1
//state
static uint16_t sample_period;
static uint32_t last_sample_time = 0;
static uint32_t last_pressed_time = 0;
static uint8_t i = 0;

void app_init(void) {
    sample_period = sample_rate_list[i];
    gpio_init();
    gpio_sample_toggle_led(i);
    serial_init(9600);
    adc_init();
    timer_init();
    sei();
}

void app_sensor_sample(void) {
    uint32_t now = get_time_ms();
    if ((now - last_sample_time) >= sample_period) {
        gpio_toggle_led();
        uint16_t value = adc_sample(ADC_CHANNEL);
        serial_write_uint(value);
        serial_write_char('\n');

        last_sample_time = now;
    }
}

void app_handle_sampling_button(void) {
    uint32_t now = get_time_ms();
    
    if (isr_sampling_button_flag) {
        //prevents triggering multiple times by miss clicking
        if ((now - last_pressed_time) >= pressed_period) {
            gpio_sample_toggle_led(i);
            sample_period = sample_rate_list[i++ % 3];
            last_pressed_time = now;
        }
        last_sample_time = now;
        //reset flag
        isr_sampling_button_flag = 0;
        if (i == 3) {i = 0;}
    }
}