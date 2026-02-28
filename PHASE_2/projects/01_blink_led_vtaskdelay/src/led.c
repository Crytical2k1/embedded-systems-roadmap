#include "driver/gpio.h"
#include "led.h"

#define LED_GPIO GPIO_NUM_2

void led_init(void) {
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}

void led_on(void) {
    gpio_set_level(LED_GPIO, 1); //led on
}
void led_off(void) {
    gpio_set_level(LED_GPIO, 0); //led off
}

void led_toggle(void) {
    static int state = 0;
    state = !state;
    gpio_set_level(LED_GPIO, state);
}