#include "isr_flags.h"
#include <stdint.h>

//initializes the event flag to 0 as a volatile variable as can be
//changed by the ISR
volatile uint8_t isr_event_flag = 0;

//varible to prevent debouncing when setting the ISR flag by pressing the button
volatile uint32_t last_interrupt_time = 0;

