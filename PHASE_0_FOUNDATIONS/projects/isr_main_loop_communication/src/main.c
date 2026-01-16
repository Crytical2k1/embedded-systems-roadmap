#include <avr/interrupt.h>
#include "gpio.h"
#include "isr_flags.h"
#include "timer.h"

int main(void) {
  //initailizacion
  gpio_init();
  timer_init();
  sei(); // enable global interrupts
  //super-loop
  while(1) {
    //flag utilization
    if (isr_event_flag) {
      //clear_flag_safely
      isr_event_flag = 0;
      //handle_event
      gpio_toggle_led();
    }
  }
}