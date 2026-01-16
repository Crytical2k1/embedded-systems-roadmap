# ADC data acquisition

This is the third embedded project to put into practice some of the knowledge i got from the theory of the phase 0, including bit operations, register and port designation, serial communication and sampling frecuency.

## Project idea

Generate code that will collect ADC data at a given frecuency, convert it into digital and send it through serial communication.

## Hardware

Elegoo/Arduino Uno R3 (ATMega328P)
Potenciometer connected to analog pin 1

## Process of code development

1. Timer setup
   .c file used to create a timer to increase a counter every 1ms, used in main for the frecuency of data sampling.

2. Serial setup
   Initializes the serial by setting up the baud rate and enabling the transmision. And stores the functions to convert digital information into ASCII in order to read the values.

3. ADC setup
   Initializes the analog-digital converter and handles the input conversion.

4. Main setup
   Initializes each block, timer, serial and ADC. Inside the main loop we check the time elapsed and trigger the adc sample and serial sent of the data with a given frecuency using the timer module. The values of the ADC_sample is a value between 0-1023 but it was rescaled to land between 0-5V which is the Arduino output voltage used as reference for the ADC module.

## Problems encountered

Main issue was understanding the new logic for ADC registers.

An factor added to the code was translating the outcome value from the adc_sample function that was a reference value between 0-1023 which means little to our eye, so i had to add a conversion scaler to convert it to voltage that is easier to undersand.
