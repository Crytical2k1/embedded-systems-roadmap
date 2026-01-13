# BLINK LED WITHOUT DELAY

This is my first embedded project to put into practice some of the knowdelge i got from the theory of the phase 0 including bit operations, register and port designation, creation of a timer and interrupt utilization.

## Project idea
Generate code that will blink an LED without using delay(), and making use of interrupts and timer flow.

## Hardware
Elegoo/Arduino Uno R3 (ATMega328P)

## Process of code development

1. GPIO setup
   .h files is easy to implement as they are just stating the functions (forward declaration) i will develop in .c files.
   I had to make use of proper header guards to make compiling safe.

   .c file included GPIO initialization, stating the port used to blink the onboard led. After datasheet check, PORTB at bit 5 was the one to use.
   In the initialization function (gpio_init()) i just had to state the direction of the port to output.
   An in the toggle led function (gpio_toggle_led()) implement the bitwise operation to toggle the corresponding bit.

2. Timer setup
   .c file included the timer_init() function that is in charge of starting the onboard MCU timer, this time used the Timer1 (16-bit), the reason of this choice is that Timer0 is a core timer of Arduino with its own registers and protocols, while Timer1 gives more freedom when customizing the properties of the timer. Important flow to follow when initializing the timer: stop the timer, select mode, set compare value, enable interrupt, start timer.
   On the same file the ISR function that will run every time the interrups fires up, this will increase the system time by one (every 1ms). We kept it simple inside the ISR (read-modify-write).
   And finally a function to get the current time of the system, as the value is used in the ISR and the size of the time value chosen is bigger than the architecture of the MCU, taking more than 1 instruction to copy it, so we had to make the code safe as is a critical section, by disabling the interrupts.

3. Main
   simple code implemented here, initialization of the functions GPIO and timer
   And getting into the super-loop that will check if the time elapsed from last blink to the current time is more than the blinking period, running the toggling led function just when needed.

## Problems encountered
Main issue was understanding the datasheet and knowing what registers to use to achive the functionality that i needed.

Figure out the proper structure of the files and order of declaration of each function. As i advanced on the project more of the theory was coming to place and making sense by putting it together.
