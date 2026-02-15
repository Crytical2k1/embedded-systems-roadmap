#include <avr/io.h>

#define LED_PORT PORTD
#define LED_DDR DDRD
#define LED_BIT_CLOCK 6
#define LED_BIT_LATCH 4
#define LED_BIT_DATA 2

#define TOP_G 0
#define TOP_R 4
#define BOTTOM_G 1
#define BOTTOM_R 5
#define LEFT_G 2
#define LEFT_R 6
#define RIGHT_G 3
#define RIGHT_R 7

void led_init(void) {
    //set pin direction as output
    LED_DDR |= (1 << LED_BIT_CLOCK) | (1 << LED_BIT_LATCH) | (1 << LED_BIT_DATA);
}

void shift595(uint8_t data) {
    for (int8_t i = 7; i >= 0; i--) {
        //set DATA bit
        if (data & (1 << i)) {
            //set to 1 the bit at place i
            LED_PORT |= (1 << LED_BIT_DATA);
        } else {
            //clear bit at place i
            LED_PORT &= ~(1 << LED_BIT_DATA);
        }
        // pulse CLOCK
        LED_PORT |= (1 << LED_BIT_CLOCK);
        LED_PORT &= ~(1 << LED_BIT_CLOCK);
    }

    //pulse LATCH
    LED_PORT |= (1 << LED_BIT_LATCH);
    LED_PORT &= ~(1 << LED_BIT_LATCH);
}

uint8_t build_led_state(float roll, float pitch)
{
    uint8_t leds = 0;

    // Pitch up
    if (pitch < -5)
        leds |= (1 << TOP_G);

    if (pitch < -25)
        leds |= (1 << TOP_R);

    // Pitch down
    if (pitch > 5)
        leds |= (1 << BOTTOM_G);

    if (pitch > 25)
        leds |= (1 << BOTTOM_R);

    // Roll left
    if (roll < -10)
        leds |= (1 << LEFT_G);

    if (roll < -45)
        leds |= (1 << LEFT_R);

    // Roll right
    if (roll > 10)
        leds |= (1 << RIGHT_G);

    if (roll > 45)
        leds |= (1 << RIGHT_R);

    return leds;
}