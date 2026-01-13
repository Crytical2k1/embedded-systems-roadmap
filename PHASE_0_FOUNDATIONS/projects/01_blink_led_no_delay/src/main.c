#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "timer.h"

#define BLINK_PERIOD_MS 100

//main loop
int main(void) {
  //initialize the gpio ports and start the Timer1
  gpio_init();
  timer_itit();
  sei(); // enable global interrupts

  uint32_t last_toggle_time = 0;

  while(1) {
    uint32_t now = timer_get_ms();

    if ((now-last_toggle_time) >= BLINK_PERIOD_MS) {
      gpio_toggle_led();
      last_toggle_time = now;
    }
  }
}