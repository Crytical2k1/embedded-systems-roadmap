# ISR - MAIN LOOP Communication

This is the second embedded project to put into practice some of the knowledge i got from the theory of the phase 0, including bit operations, register and port desgincation, event handling with flag and interrupt.

## Project idea

Generate code that will toggle an LED when pressing a button using an external interrupt (ISR) to set a volatile flag, and the main loop to poll that flag and toggle the LED.

## Hardware

Elegoo/Arduino Uno R3 (ATMega328P)
Led connected to digital pin 13
Button connected to digital pin 2

## Process of code development

1. GPIO setup
   .h files is easy to implement as they are just stating the functions (forward declaration) i will develop in .c files.
   I had to make use of proper header guards to make compiling safe.

   .c file included GPIO initialization, stating the port used to toggle the led and stating the port of the button. After datasheet check, PORTB at bit 5 was the one to use for the led, and PORTD at bit 2 for the button as is hardwired to the the (external interrupt 0).
   In the initialization function (gpio_init()) i just had to state the direction of the ports and configure the interrupt (INT0).
   The ISR function that will generate the event_flag used in the main loop. This includes debounce code, to not fire the ISR many times when pressing the button for long time.
   And in the toggle led function (gpio_toggle_led()) implement the bitwise operation to toggle the corresponding bit.

2. Timer setup
   .c file used to create a timer so that i can handle the debounce for the button. Same timer as in my previous project "blink LED without delay()"

3. ISR flag
   .c file that just initializes the event flag variable as a volatile because it is modified by the ISR. Also needed to initialize the debounce variable in here.

4. Main
   simple code implemented here, initialization of the functions GPIO and timer
   And getting into the super-loop that will check if the flag was set and if so, clear safely the event flag value and toggle the led.

## Problems encountered

First issue was trying to use Arduino pin 12 to connect the button, but the interrupt of the MCU has no access to this pin, so had to change it to digital pin 2 as i was using INT0.

An extra layer of difficulty came after setting up the code without the debouncing protection, as every time the button was pressed, the outcome was random, coming to the conclusion that as the button is pressed the interrupt was firing the ISR multiple times toggling the led repetitively (turning it on with a lower intensity as it was averaging On and Off constantly) and by releasing the button the outcome could be any state.
Although the debouncing fixed the led on at lower intensity, it still has some issues when pressing for long time, as the human timing of clicking the button must match properly the code behaviour, so if you press multiple times in a short period of time might break the functionality and also pressing for longer that the DEBOUNCE_TIME set up.

For now, i achieved the purpose of this project that is ISR - main loop communication, so the button situation is something that can be tackled later on.
