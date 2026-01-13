#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

//forward declare the function that will configure the hardware timer
void timer_itit(void);
//forward declare the function that will return the system time
uint32_t timer_get_ms(void);


#endif 