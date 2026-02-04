#include <stdint.h>
#include "isr_flags.h"

//initializes the flag to 0 for the button input interrupt that changes sampling rate
volatile uint8_t isr_sampling_button_flag = 0;
