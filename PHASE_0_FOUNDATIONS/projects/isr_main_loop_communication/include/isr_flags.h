#ifndef ISR_FLAGS_H
#define ISR_FLAGS_H

#include <stdint.h>

//Flag set by ISR, polled and cleared in main loop
extern volatile uint8_t isr_event_flag;
//using extern as this variable exists somewhere else and we
//dont want to allocate storage

extern volatile uint32_t last_interrupt_time;



#endif