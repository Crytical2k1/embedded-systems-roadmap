#ifndef GPIO_H //check if GPIO_H was defined, if not implements the code bellow in the compiler
#define GPIO_H

//forward declare the function that will initialize the PORTs
void gpio_init(void);
//forware declare the function that will toggle the led ON-OFF
void gpio_toggle_led(void);

#endif